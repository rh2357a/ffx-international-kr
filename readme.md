# ffx-international-kr

PS2 파이널판타지10 인터내셔널 한글 번역 프로젝트

## 패치 정보

* 게임 ID: SLPM-67513
* 사용 폰트: 돋움체(일반 텍스트), Noto Sans(그래픽)
* 번역 방식: 일본어 번역

## 사용, 번역 도구 정보

### 번역

* [바른 한글](https://nara-speller.co.kr/speller)
* [ChatGPT](https://chatgpt.com/)
* [구글 번역](https://translate.google.com/)
* [NAVER 사전](https://dict.naver.com/)

### 도구

* [Ghidra](https://github.com/NationalSecurityAgency/ghidra), [ghidra-emotionengine](https://github.com/chaoticgd/ghidra-emotionengine-reloaded): 프로그램 분석
* [armips](https://github.com/Kingcom/armips): 어셈블리 코드 패치
* [xdelta3](https://github.com/jmacd/xdelta): 패치 파일 생성
* [RK-Translations](https://www.rk-translations.cz): 체코어 번역 프로젝트 ([대부분의 텍스트, 그래픽 도구](<https://www.romhacking.net/utilities/1390>))

## 한국어화 빌드 방법

(1) 게임의 ISO 파일을 프로젝트 최상단에 이름을 `base.iso`으로 복사  
(2) `build.bat` 또는 `build.multilang.bat` 실행  
(3) 성공 시, `ffx_international_kr.iso`가 생성됩니다.

## 빌드 정보

### 빌드

* `build.bat`: 한국어화 빌드 (일반, 실험용)
* `build.multilang.bat`: 한국어화 빌드 (한국어/영어 선택 버전)

### 테스트

* `build.test.system.bat`: 이벤트를 제외한 텍스트 테스트 빌드
* `build.test.gfx.bat`: 그래픽 테스트 빌드
* `build.test.jpn.bat`: 일어판 내용 테스트 빌드

## 일본어 음성 패치?

FFX 오리지널 버전 (JP)의 파일 17\~101번을 복사하여  
파일 18\~102번으로 이름을 바꾸어 붙여놓기만 하면 일본어 음성을 그대로 쓸 수 있습니다.  
패치 용량이 크고, 음성에 따른 텍스트 수정이 까다롭기 때문에 패치 제작은 무리입니다.
