Assignment 5 Writeup
=============

My name: 강두경

My POVIS ID: lapis

My student ID (numeric): 20190065

This assignment took me about 7 hours to do (including the time on studying, designing, and writing the code).

If you used any part of best-submission codes, specify all the best-submission numbers that you used (e.g., 1, 2): []

- **Caution**: If you have no idea about above best-submission item, please refer the Assignment PDF for detailed description.

Program Structure and Design of the NetworkInterface:
- `std::unordered_map<uint32_t, std::pair<EthernetAddress, size_t>> addr_cache`에 30초 동안 ip address와 ethernet address,저장된 당시의 틱을 저장합니다.
- `std::unordered_map<uint32_t, size_t> arp_cooltimes`: ARP request를 보낼 때 5초 동안 중복된 리퀘스트를 보내지 않기 위해 맵에 저장합니다.
- `std::unordered_map<uint32_t, std::queue<InternetDatagram>> unknown_arp_queue`에 ARP REPLY를 받은 후 보낼 패킷을 저장하는 queue입니다.


Implementation Challenges:
- ARPMessage를 직접 만들어야 한다는 점을 나중에 깨달았습니다.
- ARP REPLY가 왔을 때 queue에 저장되어 있는 데이터그램을 보내야 한다는 개념을 나중에 확인했습니다.

Remaining Bugs:
- error가 있는 데이터그램에 대해 확인하지 못했습니다.

- Optional: I had unexpected difficulty with: assn4보다는 쉬울 것으로 예상했습니다.

- Optional: I think you could make this assignment better by: 에러가 있는 데이터그램에 대해 예외처리를 조금 더 해줄 수 있습니다.

- Optional: I was surprised by: [describe]

- Optional: I'm not sure about: [describe]
