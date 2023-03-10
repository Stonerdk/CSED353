#include "stream_reassembler.hh"

#include <iostream>
// Dummy implementation of a stream reassembler.

// For Lab 1, please replace with a real implementation that passes the
// automated checks run by `make check_lab1`.

// You will need to add private members to the class declaration in `stream_reassembler.hh`

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;

StreamReassembler::StreamReassembler(const size_t capacity) : auxilary(), _output(capacity), _capacity(capacity) {}

//! \details This function accepts a substring (aka a segment) of bytes,
//! possibly out-of-order, from the logical stream, and assembles any newly
//! contiguous substrings and writes them into the output stream in order.
void StreamReassembler::push_substring(const string &data, const size_t index, const bool eof) {
    const size_t aux_start = _output.buffer_size() + _output.bytes_read();
    const size_t full_size = _capacity + _output.bytes_read();
    const size_t data_end = index + data.length();

    if (eof) {
        if (full_size >= data_end) {
            _capacity = data_end - _output.bytes_read();
            eof_flag = true;
        }
    }
    if (data_end > aux_start && index < full_size) {
        string _data = data.substr(max(0, static_cast<int>(aux_start - index)), min(data.length(), full_size - index));
        size_t _idx = max(index, aux_start);
        for (const char &c : _data) {
            auxilary[_idx++] = c;
        }
        string input = "";
        for (size_t i = aux_start; auxilary.count(i) > 0; i++) {
            input.push_back(auxilary[i]);
            auxilary.erase(i);
        }
        _output.write(input);
    }

    if (eof_flag && empty())
        _output.end_input();
}

size_t StreamReassembler::unassembled_bytes() const { return auxilary.size(); }

bool StreamReassembler::empty() const { return auxilary.empty(); }
