# ffx-international-kr

PS2 파이널판타지10 인터내셔널 한글 번역 프로젝트

## 패치 정보

* 게임 ID: SLPM-67513
* 텍스트 폰트: 돋움체
* 그래픽 폰트: Noto Sans KR
* 번역 방식: 일본어 번역

## 사용, 번역 도구 정보

### 번역

* [바른 한글](https://nara-speller.co.kr/speller): 맞춤법 체크
* [ChatGPT](https://chatgpt.com/)
* [구글 번역](https://translate.google.com/)
* [NAVER 사전](https://dict.naver.com/)

### 도구

* [Ghidra](https://github.com/NationalSecurityAgency/ghidra), [ghidra-emotionengine](https://github.com/chaoticgd/ghidra-emotionengine-reloaded): 프로그램 분석
* [armips](https://github.com/Kingcom/armips): 어셈블리 코드 패치
* [xdelta3](https://github.com/jmacd/xdelta): 패치 파일 생성
* [RK-Translations](https://www.rk-translations.cz): 체코어 번역 프로젝트 ([대부분의 텍스트, 그래픽 도구](<https://www.romhacking.net/utilities/1390>))

## 패치 생성 및 테스트

(1) 게임의 ISO 파일을 프로젝트 최상단에 `base.iso`로 복사  
(2) `build.bat` 실행  
(3) 성공 시, `ffx_international_kr.iso`와 `ffx_international_kr.xdelta`가 생성됩니다.

## 빌드 정보

* `build.bat`: 한국어화 빌드 및 패치 생성
* `build.test.system.bat`: 이벤트 텍스트를 제외하여 빌드
* `build.test.etc_gfx.bat`: 기타 텍스트와 그래픽만 빌드
* `build.test.jpn.bat`: 일어판 확인용 빌드

## 번역 규칙

* 인물명 중 {Name} 형태로 문서화 된 경우가 있는데 실제 이름을 참고하여 받침 유무를 판단
* 듀얼쇼크의 버튼 모양: 받침이 있는것으로 간주 (xx 버튼)
* 조사 구분: '는(은)', '를(을)', '과(와)', '(이)가', '(으)로' 으로 표기

## 일본어 음성 패치?

FFX 오리지널 버전 (JP)의 파일 17\~101번을 그대로 복사하여  
해당 프로젝트의 파일 18\~102번으로 이름을 바꿔 대체만 하면  
일본어 음성을 그대로 쓸 수 있습니다.  
하지만 텍스트는 영문 음성 기준으로 번역되어 있기 때문에 따로 수정일 필요합니다.  
그리고 배포 시 용량이 너무 크기 때문에 따로 패치로 만들지 않을 예정
