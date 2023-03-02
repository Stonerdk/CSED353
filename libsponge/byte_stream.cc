#include "byte_stream.hh"

// Dummy implementation of a flow-controlled in-memory byte stream.

// For Lab 0, please replace with a real implementation that passes the
// automated checks run by `make check_lab0`.

// You will need to add private members to the class declaration in `byte_stream.hh`

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;

ByteStream::ByteStream(const size_t capacity) {
    stream = "";
    _error = false;
    ended = false;
    _bytes_written = 0;
    _bytes_read = 0;
    _capacity = capacity;
}

size_t ByteStream::write(const string &data) {
    const size_t size = min(data.size(), remaining_capacity());
    if (input_ended()) {
        _error = true;
        return 0;
    } else {
        stream += data.substr(0, size);
        _bytes_written += size;
        return size;
    }
}

//! \param[in] len bytes will be copied from the output side of the buffer
string ByteStream::peek_output(const size_t len) const {
    const size_t size = min(len, buffer_size());
    return stream.substr(0, size);
}

//! \param[in] len bytes will be removed from the output side of the buffer
void ByteStream::pop_output(const size_t len) {
    const size_t size = min(len, buffer_size());
    stream.erase(0, size);
    _bytes_read += size;
}

//! Read (i.e., copy and then pop) the next "len" bytes of the stream
//! \param[in] len bytes will be popped and returned
//! \returns a string
std::string ByteStream::read(const size_t len) {
    const string &peek = peek_output(len);
    pop_output(len);
    return peek;
}

void ByteStream::end_input() { ended = true; }

bool ByteStream::input_ended() const { return ended; }

size_t ByteStream::buffer_size() const { return stream.size(); }

bool ByteStream::buffer_empty() const { return stream.empty(); }

bool ByteStream::eof() const { return buffer_empty() && input_ended(); }

size_t ByteStream::bytes_written() const { return _bytes_written; }

size_t ByteStream::bytes_read() const { return _bytes_read; }

size_t ByteStream::remaining_capacity() const { return _capacity - buffer_size(); }
