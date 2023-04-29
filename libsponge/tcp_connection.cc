#include "tcp_connection.hh"

#include <iostream>

// Dummy implementation of a TCP connection

// For Lab 4, please replace with a real implementation that passes the
// automated checks run by `make check`.

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;

size_t TCPConnection::remaining_outbound_capacity() const { return _sender.stream_in().remaining_capacity(); }

size_t TCPConnection::bytes_in_flight() const { return _sender.bytes_in_flight(); }

size_t TCPConnection::unassembled_bytes() const { return _receiver.unassembled_bytes(); }

size_t TCPConnection::time_since_last_segment_received() const { return _tick; }

void TCPConnection::segment_received(const TCPSegment &seg) {
    auto& header = seg.header();

    _tick = 0;

    if (header.rst) 
        return set_error();

    _receiver.segment_received(seg);

    if (header.ack)
        _sender.ack_received(header.ackno, header.win);

    if (seg.length_in_sequence_space() == 0 && _receiver.ackno().has_value() && (header.seqno == _receiver.ackno().value() - 1))
        _sender.send_empty_segment();

    if (seg.length_in_sequence_space()) {
        _sender.fill_window();
        if (_sender.segments_out().empty()) 
            _sender.send_empty_segment();
        send_segment(false);
    }

    _linger_after_streams_finish &= _sender.stream_in().eof() || !inbound_ended();
}

bool TCPConnection::active() const {
    const bool preq = inbound_ended() && _sender.stream_in().eof() && !bytes_in_flight();
    const bool passive = _tick >= 10 * _cfg.rt_timeout || !_linger_after_streams_finish;
    return !_err && !(preq && passive);
}

size_t TCPConnection::write(const string &data) {
    size_t size = _sender.stream_in().write(data);
    _sender.fill_window();
    send_until_empty();
    return size;
}

//! \param[in] ms_since_last_tick number of milliseconds since the last call to this method
void TCPConnection::tick(const size_t ms_since_last_tick) {
    if (!active()) return;

    _tick += ms_since_last_tick;

    _sender.tick(ms_since_last_tick);

    if (_sender.consecutive_retransmissions() > TCPConfig::MAX_RETX_ATTEMPTS) {
        _sender.send_empty_segment();
        send_segment(true);
        return set_error();
    }

    send_until_empty();
}

void TCPConnection::end_input_stream() {
    if (!active()) return;
    _sender.stream_in().end_input();
    _sender.fill_window();
    send_until_empty();
}

void TCPConnection::connect() {
    _sender.fill_window();
    send_until_empty();
}

TCPConnection::~TCPConnection() {
    try {
        if (active()) {
            cerr << "Warning: Unclean shutdown of TCPConnection\n";
        
            // Your code here: need to send a RST segment to the peer
            _sender.send_empty_segment();
            send_segment(true);
            set_error();

        }
    } catch (const exception &e) {
        std::cerr << "Exception destructing TCP FSM: " << e.what() << std::endl;
    }
}

// private
void TCPConnection::send_segment(bool rst) {
    TCPSegment seg = _sender.segments_out().front();
    _sender.segments_out().pop();

    TCPHeader& header = seg.header();

    if (_receiver.ackno().has_value()) {
        header.ack = true;
        header.ackno = _receiver.ackno().value();
    }

    header.win = _receiver.window_size();
    header.rst = rst;

    _segments_out.push(seg);   
}

void TCPConnection::send_until_empty() {
    while (!_sender.segments_out().empty()) {
        send_segment(false);
    }
}

void TCPConnection::set_error() {
    inbound_stream().set_error();
    _sender.stream_in().set_error();
    _err = true;
}


bool TCPConnection::inbound_ended() const { 
    return unassembled_bytes() == 0 && _receiver.stream_out().input_ended(); 
}