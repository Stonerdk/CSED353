#include "tcp_receiver.hh"

// Dummy implementation of a TCP receiver

// For Lab 2, please replace with a real implementation that passes the
// automated checks run by `make check_lab2`.

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;

void TCPReceiver::segment_received(const TCPSegment &seg) {
    const TCPHeader &header = seg.header();
    const string payload = seg.payload().copy();

    if (!is_syn && header.syn)
        isn = WrappingInt32(header.seqno);

    is_syn |= header.syn;
    is_fin = (is_fin & !header.syn) | header.fin;

    const uint64_t checkpoint = _reassembler.stream_out().bytes_written();
    const WrappingInt32 seqno(header.seqno - !header.syn);
    const uint64_t pos = unwrap(seqno, isn, checkpoint);

    _reassembler.push_substring(payload, pos, header.fin);
}

optional<WrappingInt32> TCPReceiver::ackno() const {
    if (!is_syn)
        return std::nullopt;

    const uint64_t checkpoint = _reassembler.stream_out().bytes_written();
    uint64_t size = checkpoint + is_syn + (is_fin && _reassembler.empty());
    return wrap(size, isn);
}

size_t TCPReceiver::window_size() const { return _capacity - _reassembler.stream_out().buffer_size(); }
