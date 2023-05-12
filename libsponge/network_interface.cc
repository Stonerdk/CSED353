#include "network_interface.hh"

#include "arp_message.hh"
#include "ethernet_frame.hh"

#include <iostream>

// Dummy implementation of a network interface
// Translates from {IP datagram, next hop address} to link-layer frame, and from link-layer frame to IP datagram

// For Lab 5, please replace with a real implementation that passes the
// automated checks run by `make check_lab5`.

// You will need to add private members to the class declaration in `network_interface.hh`

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;

//! \param[in] ethernet_address Ethernet (what ARP calls "hardware") address of the interface
//! \param[in] ip_address IP (what ARP calls "protocol") address of the interface
NetworkInterface::NetworkInterface(const EthernetAddress &ethernet_address, const Address &ip_address)
    : _ethernet_address(ethernet_address), _ip_address(ip_address)
     {
    cerr << "DEBUG: Network interface has Ethernet address " << to_string(_ethernet_address) << " and IP address "
         << ip_address.ip() << "\n";
}

//! \param[in] dgram the IPv4 datagram to be sent
//! \param[in] next_hop the IP address of the interface to send it to (typically a router or default gateway, but may also be another host if directly connected to the same network as the destination)
//! (Note: the Address type can be converted to a uint32_t (raw 32-bit IP address) with the Address::ipv4_numeric() method.)
void NetworkInterface::send_datagram(const InternetDatagram &dgram, const Address &next_hop) {
    // convert IP address of next hop to raw 32-bit representation (used in ARP header)
    const uint32_t next_hop_ip = next_hop.ipv4_numeric();
    
    if (addr_cache.find(next_hop_ip) == addr_cache.end()) {
        if (arp_cooltimes.find(next_hop_ip) != arp_cooltimes.end()) return;
        ARPMessage arp;
        // arp.protocol_type = EthernetHeader::TYPE_ARP;
        arp.opcode = ARPMessage::OPCODE_REQUEST;
        arp.sender_ethernet_address = _ethernet_address;
        arp.sender_ip_address = _ip_address.ipv4_numeric();
        arp.target_ip_address = next_hop_ip;

        EthernetFrame arp_frame;
        arp_frame.payload() = arp.serialize();
        arp_frame.header().src = _ethernet_address;
        arp_frame.header().dst = ETHERNET_BROADCAST;
        arp_frame.header().type = EthernetHeader::TYPE_ARP;
        _frames_out.push(arp_frame);

        arp_cooltimes[next_hop_ip] = _tick;

        unknown_arp_queue[next_hop_ip].push(dgram);
    } else {
        EthernetFrame out_frame;
        out_frame.payload() = dgram.serialize();
        out_frame.header().src = _ethernet_address;
        out_frame.header().dst = addr_cache[next_hop_ip].first;
        out_frame.header().type = EthernetHeader::TYPE_IPv4;
        _frames_out.push(out_frame);
    }
}

//! \param[in] frame the incoming Ethernet frame
optional<InternetDatagram> NetworkInterface::recv_frame(const EthernetFrame &frame) {
    if (frame.header().dst != ETHERNET_BROADCAST && frame.header().dst != _ethernet_address) {
        return {};
    }

    Buffer buf = frame.payload().concatenate();
    if (frame.header().type == EthernetHeader::TYPE_IPv4) {
        InternetDatagram dgram;
        auto res = dgram.parse(frame.payload());
        if (res == ParseResult::NoError)
            return dgram;
    } else if (frame.header().type == EthernetHeader::TYPE_ARP) {
        ARPMessage msg;
        msg.parse(frame.payload());
        if (msg.target_ip_address != _ip_address.ipv4_numeric()) return {};

        addr_cache[msg.sender_ip_address] = make_pair(msg.sender_ethernet_address, _tick);

        if (msg.opcode == ARPMessage::OPCODE_REPLY) {
            auto& que = unknown_arp_queue[msg.sender_ip_address];
            while (!que.empty()) {
                EthernetFrame out_frame;
                out_frame.payload() = que.front().serialize();
                out_frame.header().src = _ethernet_address;
                out_frame.header().dst = msg.sender_ethernet_address;
                out_frame.header().type = EthernetHeader::TYPE_IPv4;
                _frames_out.push(out_frame);
                que.pop();
            }
        }

        if (msg.opcode == ARPMessage::OPCODE_REQUEST) {
            ARPMessage arp;
            // arp.protocol_type = EthernetHeader::TYPE_ARP;
            arp.opcode = ARPMessage::OPCODE_REPLY;
            arp.sender_ethernet_address = _ethernet_address;
            arp.sender_ip_address = _ip_address.ipv4_numeric();
            arp.target_ethernet_address = msg.sender_ethernet_address;
            arp.target_ip_address = msg.sender_ip_address;

            EthernetFrame ef;
            ef.payload() = arp.serialize();
            ef.header().src = _ethernet_address;
            ef.header().dst = msg.sender_ethernet_address;
            ef.header().type = EthernetHeader::TYPE_ARP;
            _frames_out.push(ef);
        }
    }

    return {};

}

//! \param[in] ms_since_last_tick the number of milliseconds since the last call to this method
void NetworkInterface::tick(const size_t ms_since_last_tick) {
    _tick += ms_since_last_tick;
    for (auto c: arp_cooltimes) {
        if (_tick >= c.second + 5000) {
            arp_cooltimes.erase(c.first);
        }
    }

    for (auto c: addr_cache) {
        if (_tick >= c.second.second + 30000) {
            addr_cache.erase(c.first);
        }
    }
}
