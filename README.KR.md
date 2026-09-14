# Pico DualSense Switch Bridge

[English](./README.md) | [简体中文](./README.CN.md)

> Raspberry Pi Pico 2 W를 사용해 Bluetooth DualSense 또는 DualSense Edge
> 컨트롤러를 PC용 유선 DualSense USB 장치나 Nintendo Switch 2용 유선
> Nintendo Switch Pro 컨트롤러 USB 장치로 연결합니다.

이 프로젝트는 [awalol/DS5Dongle](https://github.com/awalol/DS5Dongle)의
포크입니다. 기존 PC DualSense 브리지 기능을 그대로 유지하면서 실행 중 선택할
수 있는 Switch 2 호환 Nintendo Switch Pro 컨트롤러 프로필을 추가했습니다.
원본 프로젝트와 프로토콜 출처는 [크레딧과 라이선스](#크레딧과-라이선스)를
참고하세요.

## 개요

0.8.2는 v0.8.1의 오디오 초기화 heap 부족을 수정합니다. 이 문제로 기기가
응답하지 않으면서 Windows USB 인식 오류와 BOOTSEL 동작 실패가 나타날 수
있었습니다. PC 오디오·햅틱의 upstream `v0.7.2-hotfix` 기준과 Switch 모드를
유지합니다. 펌웨어 소스는 v0.8.2-rc.1과 동일합니다.

v0.8.1에서 업데이트하면 기존 설정 형식과 페어링 데이터를 유지합니다.
v0.8.0에서 업데이트하면 펌웨어 설정이 schema v5 기본값으로 초기화되므로
필요하면 BOOTSEL로 Switch 모드를 다시 선택하세요.
알려진 문제: PC 부팅 후 첫 컨트롤러 연결에서 스피커 음량이 매우 작을 수
있으며, 보고된 경우 컨트롤러 연결을 끊었다 다시 연결하면 복구됩니다.
이번 릴리스에는 이 음량 문제의 수정이 포함되지 않습니다.

컨트롤러는 Pico와 Bluetooth로 페어링된 상태를 유지합니다. Pico는 한 번에 하나의
USB 프로필만 호스트에 표시하며, 선택한 프로필을 저장해 전원을 껐다 켜도 유지합니다.

| USB 프로필 | 대상 호스트 | USB 장치 | 주요 기능 |
|---|---|---|---|
| DualSense | Windows / PC | DualSense 또는 DualSense Edge | 네이티브 입력, 터치패드, 모션, 적응형 트리거, 햅틱, 스피커, 헤드셋, 마이크 |
| Switch Pro | Nintendo Switch 2 | Nintendo Switch Pro 컨트롤러 (`057E:2009`) | 마이크 버튼을 제외한 버튼, 기본 진동; 아래 Switch 검증 현황 참고 |

Switch 프로필은 새로운 Switch 2 Pro 컨트롤러 프로토콜이 아니라 기존 Nintendo
Switch Pro 컨트롤러 프로토콜을 사용합니다. 따라서 Switch 2 설정에서
**Nintendo Switch Pro 컨트롤러 유선 통신**을 활성화해야 합니다. Switch 입력은
기존 Pro 컨트롤러와 동일한 8 ms USB 주기(125 Hz)를 사용합니다. NFC/amiibo와
Switch 2의 `C` 버튼은 지원하지 않습니다.

## Switch 검증 현황

**검증 완료 (Verified on Switch)**

- BOOTSEL 더블 클릭으로 Pro 컨트롤러 모드 전환
- 기본 진동 동작 — HD 진동 수준의 재현 정확도는 아직 검증되지 않았습니다.
- 마이크 버튼을 제외한 모든 버튼 동작

**개발 중 (Under development)**

- **자이로/모션은 심각한 버그로 현재 사용 불가입니다.**
- HD 진동 변환은 실험적 구현이며, 정확도와 완성도는 검증 중입니다.

**실기 검증 대기 (Not yet verified on Switch)**

아날로그 스틱 움직임, 마이크 버튼, 전원 재부팅 후 프로필 유지, 절전/복귀 및
재연결을 포함한 나머지 구현 동작은 아직 Switch에서 검증되지 않았습니다.
위에서 명시한 비지원 기능은 그대로 비지원 범위입니다.

## 주요 기능

아래는 구현된 기능에 대한 설명입니다. Switch 실기에서 검증된 범위는 위의
**검증 완료** 항목으로 한정됩니다.

- 🎮 실행 중 전환 가능한 DualSense 및 Nintendo Switch Pro USB 프로필
- 🔁 USB 재연결이나 전원 재부팅 후에도 선택한 프로필 유지
- 🎮 Pico 2 W를 통한 DualSense 및 DualSense Edge 전체 연결 기능
- 🌀 **Switch 자이로/모션 — 개발 중:** 심각한 버그로 현재 사용 불가입니다
- 🔊 **Switch HD 진동(HD Rumble) — 실험적 구현, 개발 중:** 비슷한 진동은 느껴지지만 정확도와 완성도는 검증 중입니다
- ✨ PC 모드에서 DualSense 네이티브 햅틱과 적응형 트리거 지원
- 🎧 컨트롤러 스피커와 3.5 mm 단자를 통한 헤드셋 오디오 출력
- 🎤 컨트롤러 마이크를 USB 오디오 입력 장치로 제공
- 📡 Bluetooth 무선 브리지
- 🔘 BOOTSEL 버튼으로 페어링, USB 프로필 변경, 플래싱 모드 진입, 페어링 삭제
- ⚡ 오버클럭 없이 기본 150 MHz 클럭으로 동작

## 시작하기

### 펌웨어 받기

두 가지 방법이 있습니다.

- **미리 빌드된 `.uf2` 다운로드** — 최신 [Releases](../../releases)에서
  `pico-dualsense-switch-bridge-<version>.uf2`를 받으세요. 다른 보드용 빌드는
  `other.board.zip`에 묶여 있고 `config_tool.py`도 함께 첨부됩니다.
- **직접 빌드** — 아래 [빌드 방법](#빌드-방법)을 참고하세요. Windows에서는
  한 번의 명령으로 빌드할 수 있습니다.

### 펌웨어 플래싱

1. Pico 2 W의 BOOTSEL 버튼을 누른 상태로 유지합니다.
2. Pico 2 W를 USB로 컴퓨터에 연결합니다.
3. USB 대용량 저장장치가 나타납니다.
4. `.uf2` 펌웨어 파일을 해당 드라이브로 끌어다 놓습니다.

> 펌웨어 실행 중에도 BOOTSEL 모드로 재부팅할 수 있습니다. [웹 설정](#설정)의
> **Reboot to Bootloader** 버튼을 누르면 물리 BOOTSEL 버튼을 누르지 않고도
> 대용량 저장장치 모드로 재부팅됩니다.

### 컨트롤러 페어링

1. DualSense 컨트롤러를 Bluetooth 페어링 모드로 전환합니다.
2. Pico 2 W가 컨트롤러를 감지하고 연결할 때까지 기다립니다.
3. 연결되면 호스트 시스템에 컨트롤러 장치가 나타납니다.

***컨트롤러가 페어링 모드일 때 Pico를 다시 연결해야 할 수도 있습니다.***

### USB 출력 모드

기본 프로필은 Windows용 **DualSense**입니다. 펌웨어가 실행 중일 때 BOOTSEL을
더블 클릭하면 **Nintendo Switch Pro 컨트롤러** 출력으로 바뀌고, 다시 더블
클릭하면 DualSense로 돌아옵니다. 선택한 프로필은 flash에 저장되고 다음 부팅 때
복원됩니다. 프로필이 바뀌면 USB가 자동으로 연결 해제된 뒤 다시 연결됩니다.
DualSense와 Pico의 Bluetooth 페어링은 유지되므로 USB 프로필을 바꿀 때 다시
페어링할 필요가 없습니다.

프로필을 선택하려고 BOOTSEL을 누른 상태로 Pico를 연결하지 마세요. RP2350 boot
ROM은 이 동작을 UF2 플래싱 진입으로 처리하므로 애플리케이션 펌웨어가 실행되지
않습니다. 프로필 선택은 펌웨어 실행 중 더블 클릭으로만 수행합니다.

Switch 2에서는 **설정 → 컨트롤러 및 액세서리**에서 **Nintendo Switch Pro
컨트롤러 유선 통신**을 활성화하세요. Switch Pro 프로필로 전환한 다음 첫 테스트
때는 Pico를 dock에서 물리적으로 분리했다가 다시 연결하세요. DualSense 모드는
Switch 2가 기본적으로 인식하지 않습니다.

**Switch 자이로/모션은 심각한 버그로 현재 사용 불가이며, 개발 중입니다.**
모션 변환 구현은 있으나 실제 사용할 수 있는 상태가 아닙니다.

**Switch HD 진동(HD Rumble)은 실험적 구현으로, 아직 개발 중입니다.** 현재
진동 데이터를 해석해 DualSense 액추에이터용 스테레오 PCM으로 합성합니다.
비슷한 진동은 느껴지지만 정확도와 완성도는 검증 중이며, HD 진동을 충실하게
재현하거나 개발을 완료한 기능으로 보기 어렵습니다. 진동 중 USB 호스트가 사라지면
500 ms watchdog이 액추에이터를 무음으로 줄입니다.

### BOOTSEL 버튼: 페어링, USB 모드 변경, 컨트롤러 삭제

펌웨어 실행 중 Pico의 **BOOTSEL 버튼**은 컨트롤러와 USB 프로필 관리 버튼으로도
동작합니다. 케이블을 뽑거나 다시 플래싱할 필요가 없습니다.

- **짧게 한 번 클릭:**
  - 컨트롤러가 연결돼 있으면 현재 컨트롤러의 연결을 끊습니다. 페어링 정보는
    유지되므로 나중에 다시 연결할 수 있습니다. 이미 페어링된 다른 컨트롤러를
    연결하고 싶을 때 사용합니다.
  - 연결된 컨트롤러가 없으면 새 컨트롤러를 페어링하기 위한 30초 스캔을
    시작합니다. 스캔 중 DualSense의 **PS + Create/Share**를 라이트바가
    깜빡일 때까지 눌러 페어링 모드로 전환하세요.
- **더블 클릭:** **DualSense**와 **Switch Pro** USB 출력을 전환하고 선택을
  저장한 뒤 USB를 다시 연결합니다. 두 번째 또는 세 번째 클릭을 기다리기 위해
  짧은 지연 후 동작합니다.
- **트리플 클릭:** **BOOTSEL로 재부팅**합니다. 연결할 때 BOOTSEL을 누르지
  않아도 동글이 USB 대용량 저장장치로 다시 나타나 새 `.uf2`를 복사할 수 있습니다.
- **길게 누르기(약 1.5초):** 연결을 끊고 **페어링된 모든 컨트롤러를 삭제**합니다.
  저장된 페어링을 삭제하고 blacklist에 넣어 전원을 다시 켜도 자동 연결되지 않게
  합니다. 확인을 위해 내장 LED가 여섯 번 깜빡입니다. 삭제한 컨트롤러를 다시
  사용하려면 **PS + Create/Share** 페어링 모드로 전환하세요.

> 트리플 클릭은 소프트웨어를 통해 bootloader로 들어가는 방법입니다. 기존처럼
> Pico를 연결할 때 **BOOTSEL을 누른 상태로 유지**해 하드웨어 방식으로 들어갈 수도
> 있습니다. 위의 클릭, 더블/트리플 클릭, 길게 누르기는 모두 **펌웨어가 이미 실행
> 중일 때** 동작합니다.

## 설정

기존 DS5Dongle과 호환되는 웹 설정에서 Pico 설정을 변경할 수 있습니다.

- 릴리스용: https://ds5.awalol.eu.org
- 개발용: https://ds5-dev.awalol.eu.org

## 커뮤니티 포크

### Audio Auto Haptics 포크 [loteran/DS5Dongle](https://github.com/loteran/DS5Dongle)

> 게임 오디오에서 실시간 햅틱 피드백을 생성합니다. Pico가 사운드 스트림을 듣고
> 저음과 충격음을 DualSense 진동으로 변환하므로 게임 자체의 햅틱 지원이 필요 없습니다.

### DS5_Bridge [SundayMoments/DS5_Bridge](https://github.com/SundayMoments/DS5_Bridge)

> 오디오, 햅틱, 트리거 강도, 조명, 버튼 remap, 단축키 등 더 많은 사용자 설정을
> 제공합니다.

### OLED Edition [MarcelineVPQ/DS5Dongle-OLED-Edition](https://github.com/MarcelineVPQ/DS5Dongle-OLED-Edition)

> awalol/DS5Dongle에 Pico-OLED-1.3 128×64 디스플레이를 선택적으로 추가한
> 포크입니다. 상태, 4-slot 다중 컨트롤러 페어링, 라이트바 색상과 효과 preset,
> 트리거 테스트, 자이로 기울기, 터치패드, 진단, CPU/클럭, Bluetooth 신호 강도,
> 오디오 VU meter, 영구 설정 메뉴 등 11개 화면과 DS5 버튼 조합 soft reboot를
> 제공합니다.

### [artzox/DS5Dongle](https://github.com/artzox/DS5Dongle)

> Awalol과 Loteran의 작업을 기반으로 simulated haptics를 확장하고 native haptics
> anti-aliasing, audio leak, trigger vibration/resistance, gyro aiming, 자동 profile
> loading 등의 기능을 추가합니다. [자세한 소개](https://github.com/awalol/DS5Dongle/issues/221)

### DS4Dongle [snipem/DS4Dongle](https://github.com/snipem/DS4Dongle)

> Raspberry Pi Pico 2 W에서 DualShock 4를 사용할 수 있게 하는 펌웨어입니다.
> DS5Dongle에서 영감을 받아 헤드셋, 마이크, 설정 기능을 제공합니다.

### [zurce/DS5Dongle-OLED](https://github.com/zurce/DS5Dongle-OLED)

## 참고 사항

컨트롤러가 연결된 뒤에만 Pico 장치가 시스템에 표시됩니다.

일부 동작은 USB를 다시 연결한 뒤 적용됩니다.

### 마이크

컨트롤러 마이크는 Windows에서 `Headset Microphone`이라는 USB 오디오 입력으로
표시됩니다. 녹음 장치로 선택한 뒤 운영체제의 입력/캡처 볼륨을 올리세요. 특히
Windows에서는 기본값이 0이거나 매우 낮을 수 있어 정상 동작 중인데도 마이크가
작동하지 않는 것처럼 보일 수 있습니다.

### 배터리 부족 LED 표시

연결된 DualSense의 배터리가 10% 이하이고 충전 중이 아니면 Pico 내장 LED가
계속 켜진 상태에서 1 Hz 점멸로 바뀝니다. 컨트롤러를 충전하거나 배터리 잔량이
올라가면 다시 계속 켜집니다. `disable_pico_led`가 설정돼 있어도 배터리 경고는
중요 알림으로 취급되어 LED 끄기 설정보다 우선합니다. 배터리가 회복되거나 충전을
시작하면 다시 설정에 따라 LED가 꺼집니다.

빌드할 때 이 기능을 제외하려면 `-DENABLE_BATT_LED=OFF`를 사용하세요. 기본값은
ON입니다.

### Pico W 버전

Pico W는 햅틱만 지원하고 스피커는 지원하지 않습니다. `-DPICO_W_BUILD=ON`으로
Pico W 펌웨어를 빌드하거나 GitHub Actions에서 미리 빌드된 펌웨어를 받을 수
있습니다.

### Waveshare RP2350B-Plus-W

[Waveshare RP2350B-Plus-W](https://www.waveshare.com/wiki/RP2350B-Plus-W)는
Pico 2 W와 같은 CYW43 silicon을 사용하는 RM2 무선 모듈, 16 MB QSPI flash,
USB-C 단자를 갖춘 RP2350B 기반 보드입니다. 다음 명령으로 빌드합니다.

```sh
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release \
      -DPICO_SDK_PATH=<sdk> -DWAVESHARE_RP2350B_PLUS_W_BUILD=ON
cmake --build build --target pico-dualsense-switch-bridge
```

또는 GitHub Actions에서 미리 빌드된 펌웨어를 받을 수 있습니다.

### USB 깨우기 기능

Wake-on-PS는 표준 펌웨어에 포함돼 있습니다. 별도의 `feat/usb-wake` 브랜치나
`pico-dualsense-switch-bridge-wake.uf2` 빌드는 없습니다. 기본값은 **꺼짐**이며,
[웹 설정](#설정)의 **Wake PC from sleep on PS button**을 켜서 활성화합니다.
활성화하면 동글이 HID 키보드 인터페이스와 USB remote wakeup 기능을 표시해
컨트롤러 버튼으로 PC를 깨울 수 있습니다. 비활성화하면 이 인터페이스를 USB에
표시하지 않습니다. 설정 방법은 [Wake-on-PS](#wake-on-ps선택-사항)를 참고하세요.

이 기능을 사용하기 전에 #60과 #61을 읽는 것을 권장합니다.

## 알려진 문제

- ⚠️ 오디오가 약간 끊길 수 있습니다.

## 성능

libopus encode/decode, resampler, Bluetooth/USB hot path 패킷 처리를 포함한 오디오
경로는 flash가 아니라 **RAM**에서 실행됩니다. 따라서 시간에 민감한 오디오
루프에서 flash fetch(XIP cache miss)로 발생하던 stall을 제거했습니다. 이전에는
오디오 encoding 성능을 맞추기 위해 RP2350을 오버클럭해야 했습니다.

현재 펌웨어는 **햅틱, 스피커, 3.5 mm 출력, 마이크를 포함한 전체 오디오 경로를
기본 150 MHz 클럭에서 실행하며 오버클럭이나 core voltage 상승이 필요 없습니다.**

> 이전 릴리스는 320 MHz @ 1.2 V가 필요했습니다. 이제 오버클럭은 필요하지
> 않습니다. 다른 보드용 빌드가 부팅되지 않는다면 `CMakeLists.txt`에서 CPU
> 주파수를 낮추거나 전압을 높이세요.

## 빌드 방법

### Windows 11(WSL 없이 한 번의 명령)

저장소 전체를 clone할 필요가 없습니다. [`tools/build-windows.ps1`](tools/build-windows.ps1)
파일만 원하는 폴더에 다운로드한 다음 **PowerShell**에서 실행하세요.

```powershell
powershell -ExecutionPolicy Bypass -File .\build-windows.ps1
```

이미 저장소를 clone했다면 저장소 루트에서 `tools\build-windows.ps1`을 실행하세요.
스크립트가 로컬 checkout을 자동으로 감지합니다.

스크립트는 CMake, Ninja, Python, Git, ARM GNU toolchain을 포함한 모든 필수 도구를
설치합니다. 기본적으로 `winget`을 사용하고, 사용할 수 없으면 portable package를
받습니다. checkout 밖에서 실행한 경우 프로젝트도 clone하며, 고정된 Pico SDK와
TinyUSB를 `%USERPROFILE%\.pico-dualsense-switch-bridge-build`에 준비합니다. 빌드된
`pico-dualsense-switch-bridge.uf2`는 스크립트 옆과 Desktop에 복사됩니다. 이미
설치된 도구는 건너뛰므로 안전하게 다시 실행할 수 있습니다.

다른 포크나 특정 ref를 빌드하려면 `-Repo <url>` / `-Ref <branch|tag>`를 사용하세요.

debug variant는 `-Variant debug`로 빌드합니다.

### 기타 플랫폼

소스에서 직접 빌드하려면 다음 단계를 따르세요.

CMake 3.18 이상이 필요합니다. 모든 펌웨어 빌드는 링크한 ELF의 초기 heap이
128 KiB 이상인지 검사합니다(스피커 처리 비활성화 시 32 KiB). v0.8.1에서 발견된
Opus 초기화 메모리 할당 실패를 방지하기 위한 검사이며, 실기 검증을 대체하지는
않습니다. 현재 오디오 경로에서 사용하지 않는 Opus 분석과 SILK 인코더 양자화
코드를 flash에 두어 기존 오디오 처리와 버퍼에 필요한 RAM을 확보합니다.

1. Pico SDK 2.2.0을 설치하고 SDK의 TinyUSB submodule을 TinyUSB 0.20.0의
   `3af1bec1a9161ee8dec29487831f7ac7ade9e189` commit으로 전환합니다.
2. 이 저장소의 submodule을 준비합니다: `git submodule update --init --recursive`
3. 표준 Pico SDK toolchain으로 구성하고 빌드합니다:
   `cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release -DPICO_SDK_PATH=<sdk>`
   실행 후 `cmake --build build --target pico-dualsense-switch-bridge`

macOS에서는 `tools/build-macos.sh`가 저장소 전용 Pico SDK checkout을 준비하고,
필요한 Homebrew 빌드 도구 설치를 물어본 뒤 submodule 초기화, TinyUSB 고정, 펌웨어
빌드를 수행합니다.

```sh
tools/build-macos.sh
```

처음부터 다시 빌드하려면 `tools/build-macos.sh --clean`, 기존 SDK checkout을
사용하려면 `--sdk-dir <path>`를 사용하세요. `--sdk-dir`을 사용하면 스크립트가 해당
SDK를 필요한 Pico SDK/TinyUSB 버전으로 checkout하기 전에 확인을 요청합니다.
Homebrew의 `arm-none-eabi-gcc` formula에 표준 C header가 없다면 전체
`gcc-arm-embedded` cask 설치를 요청하고 CMake가 그 toolchain을 사용하게 합니다.

## Xbox Game Bar(선택 사항)

[웹 설정](#설정)의 **PS button = Xbox Game Bar**를 켜면 PS 버튼이
[Wake-on-PS](#wake-on-ps선택-사항)와 같은 HID 키보드 인터페이스를 통해 키보드
단축키를 보냅니다.

- **짧게 누르기** → `Win`+`G`를 전송해 **Xbox Game Bar** overlay를 엽니다.
- **길게 누르기(750 ms 이상)** → `Win`+`Tab`을 전송해 **작업 보기**를 엽니다.

기본값은 꺼짐이며, 이 기능이나 wake 기능이 활성화된 동안에만 키보드 인터페이스가
표시됩니다.

> Game Bar overlay가 열리지만 컨트롤러 입력에 반응하지 않는다면 Windows에 최신
> 입력 stack이 없을 수 있습니다. **Microsoft GameInput**을 설치하거나 업데이트하면
> 컨트롤러 탐색 기능이 복구됩니다. 관리자 권한 명령 프롬프트에서
> `winget install Microsoft.GameInput`을 실행하거나
> [공식 문서](https://learn.microsoft.com/en-us/gaming/gdk/docs/features/common/input/overviews/input-overview)를
> 참고하세요.

## Wake-on-PS(선택 사항)

[웹 설정](#설정)의 **Wake PC from sleep on PS button**을 켜면 동글이 두 번째
HID 인터페이스인 boot keyboard와 USB remote wakeup 기능을 표시합니다. 호스트가
절전 상태일 때 컨트롤러 버튼을 누르면 **F15** 키 입력을 보내 PC를 **S3 절전**에서
깨웁니다. F15는 Windows나 일반 앱의 기본 단축키가 아니므로 예상치 못한 문자를
입력하거나 단축키를 실행하지 않습니다. 기본값은 꺼짐이며 wake 또는 Xbox Game
Bar 단축키가 켜진 경우에만 키보드 인터페이스를 표시합니다.

지원 범위는 **S3 절전만**입니다. Modern Standby(S0ix)는 지원하지 않습니다.
`powercfg /a`를 실행해 사용 가능한 절전 상태에 `Standby (S3)`가 있는지 확인하세요.

설정을 켠 뒤 **Reconnect USB**로 인터페이스를 다시 표시하고 다음을 수행합니다.

1. 장치 관리자에서 새 **HID Keyboard Device**와 상위 **USB Composite Device**의
   속성 → 전원 관리로 들어가 **이 장치를 사용하여 컴퓨터의 대기 모드를 종료할
   수 있음**을 선택합니다.
2. `powercfg /devicequery wake_armed`로 확인합니다.
3. PC를 절전 상태로 전환하고 컨트롤러의 아무 버튼이나 누릅니다. 약 1초 안에
   PC가 깨어나야 합니다.
4. 깨어난 뒤 `powercfg /lastwake`가 HID Keyboard Device를 원인으로 표시하는지
   확인합니다.

> Wake 기능을 사용하려면 컨트롤러 오디오 인터페이스(`MI_00`)에
> `SelectiveSuspendEnabled = 1`(`REG_DWORD`)도 필요합니다. Windows는 장치를
> 처음 설치할 때만 이 값을 쓰므로 실행 중 옵션을 켰다면 직접 설정해야 할 수
> 있습니다. 각 장치 instance의 `Device Parameters` key에 있습니다.
>
> ```text
> HKLM\SYSTEM\CurrentControlSet\Enum\USB\VID_054C&PID_0CE6&MI_00\<instance>\Device Parameters
>     SelectiveSuspendEnabled    (REG_DWORD) = 1
> ```
>
> `PID_0CE6`은 DualSense, `PID_0DF2`는 Edge입니다. `<instance>`는 장치와 USB
> port마다 다르므로(예: `6&212078ea&1&0000`) node가 여러 개일 수 있습니다.
> 모든 instance에 값을 설정해야 합니다. 다음 관리자 PowerShell 명령은 현재
> 존재하는 모든 instance를 처리합니다.
>
> ```powershell
> Get-ChildItem 'HKLM:\SYSTEM\CurrentControlSet\Enum\USB\VID_054C&PID_0CE6&MI_00' | ForEach-Object {
>   New-ItemProperty "$($_.PSPath)\Device Parameters" SelectiveSuspendEnabled -Value 1 -PropertyType DWord -Force }
> ```
>
> 그다음 USB를 다시 연결하거나 재부팅하세요. 장치를 제거해 Windows 장치 cache를
> 비운 뒤 다시 연결해도 Windows가 값을 다시 기록합니다.

## 로드맵

- Switch 2 실기 검증을 완료하고 간결한 USB handshake 진단 기능 추가
- endpoint backpressure와 재연결 상황에서도 Switch protocol 응답이 안정적으로
  동작하도록 개선
- Switch 자이로/모션의 알려진 버그 수정 및 실기에서 모션 보정 검증
- Switch HD 진동 변환의 개발과 정확도 검증 진행

## 커뮤니티

- 이 포크에서 재현 가능한 문제는 이 저장소의 issue tracker에 등록해 주세요.
- upstream DS5Dongle 관련 논의는
  [upstream Discord 서버](https://discord.gg/hM4ntchGCa)를 이용하세요.

## 참고 자료

- [awalol/DS5Dongle](https://github.com/awalol/DS5Dongle) — upstream 펌웨어
- [OpenStickCommunity/GP2040-CE Switch Pro driver](https://github.com/OpenStickCommunity/GP2040-CE/pull/1365) — Switch Pro USB protocol 참고 구현
- [Switch 2의 Nintendo Switch 액세서리 지원](https://support.nintendo.com/jp/switch2/accessory/controller/switch-controller/index.html) — 유선 통신 요구사항
- [rafaelvaloto/Pico_W-Dualsense](https://github.com/rafaelvaloto/Pico_W-Dualsense) — 프로젝트 아이디어
- [egormanga/SAxense](https://apps.sdore.me/SAxense) — Bluetooth 햅틱 proof of concept
- [Sony DualSense 자료](https://controllers.fandom.com/wiki/Sony_DualSense) — DualSense data report 구조
- [Paliverse/DualSenseX](https://github.com/Paliverse/DualSenseX) — 스피커 report packet
- [Nielk1의 연구 자료와 packet sample](https://github.com/egormanga/SAxense/issues/1)

## 크레딧과 라이선스

Pico DualSense Switch Bridge는
[awalol/DS5Dongle](https://github.com/awalol/DS5Dongle)을 기반으로 합니다.
Switch Pro USB 프로필은 MIT 라이선스 GP2040-CE Switch Pro driver와 관련
Nintendo Switch protocol 연구를 바탕으로 합니다. 재사용한 소스 파일의 기존
저작권 및 SPDX 고지를 유지합니다.

이 프로젝트는 [MIT License](LICENSE)로 배포됩니다. Sony Interactive
Entertainment 또는 Nintendo와 관련이 없으며 공식적으로 보증받은 프로젝트가
아닙니다.
