#include "tcp_sender.hh"

#include "tcp_config.hh"

#include <random>

// Dummy implementation of a TCP sender

// For Lab 3, please replace with a real implementation that passes the
// automated checks run by `make check_lab3`.

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;

//! \param[in] capacity the capacity of the outgoing byte stream
//! \param[in] retx_timeout the initial amount of time to wait before retransmitting the oldest outstanding segment
//! \param[in] fixed_isn the Initial Sequence Number to use, if set (otherwise uses a random ISN)
TCPSender::TCPSender(const size_t capacity, const uint16_t retx_timeout, const std::optional<WrappingInt32> fixed_isn)
    : _isn(fixed_isn.value_or(WrappingInt32{random_device()()}))
    , _initial_retransmission_timeout{retx_timeout}
    , _stream(capacity)
    , rto{retx_timeout} {}

uint64_t TCPSender::bytes_in_flight() const { return _next_seqno - ackno; }

void TCPSender::fill_window() {
    if (!is_syn) {
        is_syn = true;
        TCPSegment segment; 
        segment.header().syn = true;
        send(segment);
        return;
    }
    if (is_fin) return;

    const size_t initial_offset = ackno + max(win, 1u);
    while (!is_fin && (!_stream.buffer_empty() || _stream.eof())) {
        const size_t stream_size = _stream.buffer_size();
        const size_t remaining_size = initial_offset - _next_seqno;
        if (!remaining_size) break;
        const size_t segment_size = min({ TCPConfig::MAX_PAYLOAD_SIZE, remaining_size, stream_size });

        TCPSegment segment;
        segment.payload() = Buffer(_stream.read(segment_size));
        if (_stream.eof() && segment_size == stream_size && stream_size < remaining_size) {
            is_fin = true;
            segment.header().fin = true;
        }
        send(segment);
    }
}

//! \param ackno The remote receiver's ackno (acknowledgment number)
//! \param window_size The remote receiver's advertised window size
void TCPSender::ack_received(const WrappingInt32 ackno_, const uint16_t window_size) {
    const uint64_t next_ackno = unwrap(ackno_, _isn, _next_seqno);
    if (next_ackno > _next_seqno) return;
    ackno = max(ackno, next_ackno);
    win = window_size;

    while (!outstandings.empty() && 
       unwrap(outstandings.front().header().seqno, _isn, _next_seqno) + outstandings.front().length_in_sequence_space() <= ackno) {
        outstandings.pop();
        rto = _initial_retransmission_timeout;
        time_elapsed = r_cnt = 0;
    }
    is_tick = !outstandings.empty();
    fill_window();
 }

//! \param[in] ms_since_last_tick the number of milliseconds since the last call to this method
void TCPSender::tick(const size_t ms_since_last_tick) {
    time_elapsed += ms_since_last_tick;
    if (!outstandings.empty() && time_elapsed >= rto && is_tick) {
        _segments_out.push(outstandings.front());
        time_elapsed = 0;
        if (!win) return;
        rto <<= 1;
        r_cnt++;
    }
}

unsigned int TCPSender::consecutive_retransmissions() const {
    return r_cnt;
 }

void TCPSender::send_empty_segment() {
    TCPSegment segment;
    segment.header().seqno = wrap(_next_seqno, _isn);
    _segments_out.push(segment);
}

void TCPSender::send(TCPSegment& segment) {
    segment.header().seqno = wrap(_next_seqno, _isn);
    _next_seqno += segment.length_in_sequence_space();
    _segments_out.push(segment);
    outstandings.push(segment);
    is_tick = true;
}
