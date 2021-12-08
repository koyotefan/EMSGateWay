# EMSGateWay

### 개발년도

2021년도 03월 - 최초개발
2021년도 09월 - 부분수정

### 목적

1. EMS(Enhancement Management System, 시스템 감시/설정/통계확인등 GUI 를 포함한 운용 시스템)
2. EMS 메시지 수신 후, 시스템으로 메시지 전달 (설정 reloading 위함)
3. VM 내 연결 정보 확인 후, 현황 메시지 전송


### 주요 동작

1. Linux /proc/net/tcp 감시 및 TCP 연결 정보 확인
2. HTTP/1.1 Server 동작 (Command Line Internface 메시지 수신)
3. HTTP/1.1 Client 동작 (EMS 로의 메시지 전송)
4. Netconf(RFC4741, RFC6241) 데이터 파일 감시 또는 변경
5. DB 연결 상태 Report 수신(UDP)
