Assignment 4 Writeup
=============

My name: 강두경

My POVIS ID: lapis

My student ID (numeric): 20190065

This assignment took me about 16 hours to do (including the time on studying, designing, and writing the code).

If you used any part of best-submission codes, specify all the best-submission numbers that you used (e.g., 1, 2): []

- **Caution**: If you have no idea about above best-submission item, please refer the Assignment PDF for detailed description.

Your benchmark results (without reordering, with reordering): [0.00, 0.00]

Program Structure and Design of the TCPConnection:
- sender의 남아 있는 큐만큼 세그먼트를 보내는 부분이 자주 등장해서 send_until_empty() 함수를 이용해 처리했습니다. write, end 등 connection에서 'write' 하는 함수를 할 때 window를 fill하여 조건부로 신규 세그먼트를 생성하여 보냅니다.
- 세그먼트를 받았을 때, RST일 경우 종료, ACK일 경우 리시버의 ack_received를 실행합니다. keep-alive 처리를 하고, syn/fin/payload가 있을 때는 ack 패킷을 하나 보냅니다. (이 때 fill_window가 안 되는 조건을 잘 모르겠습니다.)
- active()는 소멸자/RST에 의해 _err=true가 되면 false가 되고, 혹은 문서에 나온 PREQ1, 2, 3을 만족하고 lingering이 아니거나 시간초과 시 false가 됩니다. 별도의 변수로 처리하지 않았습니다.
- 대부분 문서에 나와 있는 매커니즘에 충실해서 간결하고 짧게 작성하면서 올패스를 띄우려고 노력했습니다.


Implementation Challenges:
- 35번 테스트 (passive close)가 계속 실패해서 이유를 찾지 못해서 별 방법을 다 썼는데, TCPSender에서 문제가 있음을 깨달았습니다. TCPSender에서 time_elapsed를 제때 0으로 설정하지 않아 tick(1) 시 outstanding에 있던 fin 패킷이 한 번 더 보내진 게 문제였네요..
- 위 문제만 아니었다면 가장 챌린지한 부분인 Ending 파트도, 문서에 어느 정도 설명이 나와있었기에 어렵다고 느껴지지 않았을 것 같습니다.

Remaining Bugs:
테스트는 다 통과합니다.

- Optional: I had unexpected difficulty with: [describe]

- Optional: I think you could make this assignment better by: [describe]

- Optional: I was surprised by: [describe]

- Optional: I'm not sure about: [describe]
