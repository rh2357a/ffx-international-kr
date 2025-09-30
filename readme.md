# ffx-ps2-kr

PS2 파이널판타지10 인터내셔널의 한글 번역 프로젝트

## 패치 정보

* 게임 ID: SLPM-67513
* 텍스트 폰트: 돋움체
* 그래픽 폰트: Noto Sans KR
* 번역 방식: 일본어 번역

## 사용 도구 정보

* [armips](https://github.com/Kingcom/armips): 어셈블리 코드 패치
* [xdelta3](https://github.com/jmacd/xdelta): 패치 파일 생성
* 대부분의 텍스트, 그래픽 도구: 체코어 번역 프로젝트(<https://www.rk-translations.cz>, <https://www.romhacking.net/utilities/1390>)

## 패치 생성 및 테스트

(1) 게임의 ISO 파일을 프로젝트 최상단에 `base.iso`로 복사  
(2) `build.bat` 실행  
(3) 성공 시, `ffx_international_kr.iso`와 `ffx_international_kr.xdelta`가 생성됩니다.

## 빌드 정보

* `build.bat`: 한국화 패치
* `build.test.etc_gfx.bat`: 기타 텍스트와 그래픽만 한국화 패치
* `build.test.jpn.bat`: 일어판 패치
