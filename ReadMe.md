# 엔진 작업

## Release모드 실행
Libraries/Libs/Protobuf/Release/
위 디렉토리에 "libprotobuf.lib" 파일 추가후 빌드하기


## 컨벤션

### 코드

#### 코딩 컨벤션

기존 수업 코드 스타일(헝가리안)

#### 아웃풋

함수의 결과가 "return"이 아니라 파라미터의 인자를 통해 전달 될 경우 헤더에 "OUT"을 붙일 것

```cpp
void Imgui_ObjectViewer(_uint iLevel, OUT CGameObject*& pSelectedObject);
void Imgui_ProtoViewer(OUT const _tchar*& szSelectedProto);
```
OUT은 "#define OUT " 으로 컴파일시 없어집니다. 단순히 이 파라미터로 값이 출력된다는 것을 나타내기 위한 용도

#### Utility 함수

유틸함수(static함수)들은 "~~~Utils" 클래스에 모아두기

- 게임 관련 유틸 : CGameUtils
- 수학 관련 유틸 : CMathUtils

위 함목에 포함되지 않으면 각자 새로 만들기

#### Imgui 관련 기능

Imgui관련 기능의 함수중 public함수는 항상 앞에 접두사로 "Imgui_"를 붙여줍시다.

### 주석

추가된 함수 및 클래스에 주석을 주석을 써야된다고 판단되면 헤더파일에 쓴다.
//,  /**/ 둘다 가능하지만 // 권장

특별한 규칙을 없지만 주석을 읽고, 어떤 input을 넣으면 어떤 output이 나온다 정도를 알 수 있을 정도로 씁니다.

### 브랜치 명

기능 추가의 경우 해당 기능을 설명하는 브랜치 명 앞에 "feature/" 를 붙인다.
- feature/BehaviorTree_Component
- feature/Refactoring_BT

버그 수정의 경우 해당 버그 내용을 설명하는 브랜치 명 앞에 "bugfix/"를 붙인다.
- bugfix/FSMComponent_Imgui_NotShowing

### 커밋 컨벤션

[참고블로그](https://velog.io/@shin6403/Git-git-%EC%BB%A4%EB%B0%8B-%EC%BB%A8%EB%B2%A4%EC%85%98-%EC%84%A4%EC%A0%95%ED%95%98%EA%B8%B0)

#### 요약

```
태그: 제목
본문
```
의 구성으로 작성

- 태그
  - Feat : 새로운 기능 추가
  - Fix : 버그 수정
  - Docs : 문서 수정
  - Style : 코드 포맷팅, 세미콜론 누락, 코드 변경이 없는 경우
  - Refactor : 코드 리펙토링
  - Chore : 빌드 업무 수정, 패키지 매니저 수정

- 제목
    - 제목은 최대 50글자가 넘지 않도록 하고 마침표 및 특수기호는 사용하지 않는다.
    - 영문으로 표기하는 경우 동사(원형)를 가장 앞에 두고 첫 글자는 대문자로 표기한다.(과거 시제를 사용하지 않는다.)
    - 제목은 개조식 구문으로 작성한다. --> 완전한 서술형 문장이 아니라, 간결하고 요점적인 서술을 의미.

- 본문
    - 본문은 한 줄 당 72자 내로 작성한다.
    - 본문 내용은 양에 구애받지 않고 최대한 상세히 작성한다.
    - 본문 내용은 어떻게 변경했는지 보다 무엇을 변경했는지 또는 왜 변경했는지를 설명한다.
