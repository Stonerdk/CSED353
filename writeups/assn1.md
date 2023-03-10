Assignment 1 Writeup
=============

My name: Dookyung Kang

My POVIS ID: lapis

My student ID (numeric): 20190065

This assignment took me about 7 hours to do (including the time on studying, designing, and writing the code).

Program Structure and Design of the StreamReassembler:
- Assemble되지 않은 부분(auxilary)를 std::map으로 처리했다.
- 우선 auxilary의 해당하는 인덱스에 char을 저장한 후,
- 현재 stream의 끝 지점부터 최대한 stream에 write할 수 있는 부분까지 write한다.

Implementation Challenges:
- 처음에 stringAssembler의 컨셉을 이해하는 것에 시간을 많이 소요하였다.
- EOF를 처리하는 것에서 어려움을 겪었다.
- '분기'라고 볼 수 있는 index들이 많았는데 헷갈렸다. 그림을 여러 번 그려서 해결했다.

Remaining Bugs:
- capacity + bytes_read()가 정수 범위를 초과하여 overflow가 발생하는 경우

- Optional: I had unexpected difficulty with: 처음에 stringreassembler의 컨셉을 이해하는 것, EOF를 처리하는 것

- Optional: I think you could make this assignment better by: 

- Optional: I was surprised by:

- Optional: I'm not sure about: std::map보다 더 효율적이고 오버헤드가 적은 방법이 존재할까? (더 큰 스트림에 대하여), 그런 방법을 구현하는 것이 과연 효과가 있을까?
