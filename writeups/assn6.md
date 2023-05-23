Assignment 6 Writeup
=============

My name: 강두경

My POVIS ID:lapis

My student ID (numeric): 20190065

This assignment took me about 2 hours to do (including the time on studying, designing, and writing the code).

If you used any part of best-submission codes, specify all the best-submission numbers that you used (e.g., 1, 2): []

- **Caution**: If you have no idea about above best-submission item, please refer the Assignment PDF for detailed description.

Program Structure and Design of the Router:
우선, Router는 Longest-prefix-match가 원칙이므로 가장 긴 prefix부터 먼저 순회해서,
해당하는 인터페이스가 있다면 바로 데이터그램을 보내고 종료하도록 디자인하는 것이 효율적이라 생각했다.

따라서, **prefix의 길이**에 따라서 `std::greater<int>`를 통해 긴 길이부터 순회할 수 있도록 일종의 버킷 역할을 하는 std::map을 만들었다.
이 map의 각 value는 다시, route의 prefix를 키로 가지고 next_hop과 interface_num의 페어를 value로 가지는 unordered_map으로 이루어져 있다.
따라서 각 버킷을 순회하면, 버킷에서 해싱을 통해 바로 원하는 라우트의 next_hop과 interface_num을 가져올 수 있다. 
만약 없으면 다음 버킷으로 순회하고, 있으면 데이터그램을 보내고 함수를 종료한다. 
그 결과 라우팅의 시간복잡도는 최대 O(1)이 되며 하나를 찾으면 뒤에 거를 찾지 않아도 되어 중복 서칭을 방지한다.

Implementation Challenges:
효율적인 데이터 구조를 만드는 데 욕심이 들었으며 타입이 복잡해서 시간이 좀 걸렸던 점
`uint8_t`가 cerr로 출력시 잘 출력이 안 되었던 점
익숙하지 않은 구조화 바인딩 테크닉을 사용해보려 한 점
`uint32_t >> 32`가 문제가 될 수 있어 0으로 예외처리한 점
unsigned일 경우 상수 리터럴 작성 시에 u를 뒤에 표기해주어야 안정적이라는 점

Remaining Bugs:
[]

- Optional: I had unexpected difficulty with: 친구는 1시간 걸렸다는데 저는 두시간 걸렸습니다. 문제가 쉬울수록 완벽하게 풀고싶어하는 경향이 있어서 그런듯하네요

- Optional: I think you could make this lab better by: 프로그래밍 레벨에서의 병목을 줄여서 다음에는 비슷한 난이도의 문제는 1시간컷을 내고 싶네요. 

- Optional: I was surprised by: [describe]

- Optional: I'm not sure about: [describe]
