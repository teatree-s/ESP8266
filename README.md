<a name="readme-top"></a>

<!-- ABOUT THE PROJECT -->

# 1. プロジェクトについて

TCP/IP スタックとマイクロコントローラを備えた Wi-Fi モジュールである Espressif Systems 社の ESP8266 を対象としたプロジェクトです。  
ESP8266 は、Arduino IDE を使って、C/C++をベースとした Arduino 言語でプログラム開発ができます。  
ここでは、実際に Arduino IDE で作成したスケッチを展開します。

<p align="right">(<a href="#readme-top">back to top</a>)</p>

<!-- ABOUT THE TARGET -->

# 2. ESP8266 仕様

- 32 ビット RISC CPU: Tensilica Xtensa LX106 running at 80 MHz\*
- 命令 RAM 64KiB、データ RAM 96KiB
- 外部 QSPI フラッシュメモリ - 512 KiB ～ 4 MiB\*（16MiB までサポート）
- IEEE 802.11 b/g/n Wi-Fi
- 16 ピンの GPIO
- SPI, I²C,
- DMA 付きの I²S（GPIO とピン共用）
- 専用のピン上の UART と GPIO2 で有効化できる送信専用 UART
- 10 ビット ADC 1 基

<p align="right">(<a href="#readme-top">back to top</a>)</p>

<!-- GETTING STARTED -->

# 3. さあ、はじめよう！

本プロジェクトのスケッチを使用するには、Arduino IDE 開発環境と ESP8266 が動作するための設定が必要です。  
以下の手順で環境を構築ください。

## 3.1. Arduino IDE のインストール

Arduino の HP より環境に応じた Arduino IDE をダウンロードしてインストールしてください。

https://www.arduino.cc/en/software

Windows を使用している場合には右の「DOWNLOAD OPTIONS」から「Windows Win 7 and newer」を選択です。  
その後、すぐにダウンロードする場合には「JUST DOWNLOAD」を選択します。

## 3.2. ESP8266 の環境構築

公式サイトの「Installing with Boards Manager」から確認頂くのが良いと思いますが、以下にも簡単に記載しておきます。

https://github.com/esp8266/Arduino

Arduino では、ボード マネージャーを使用してサードパーティのプラットフォーム パッケージをインストールできます。

1. Arduino を起動し、[設定] ウィンドウを開きます。
2. [ファイル] > [環境設定] > [追加のボードマネージャの URL] フィールドに https://arduino.esp8266.com/stable/package_esp8266com_index.json を追加します。
3. [ツール] > [ボード] > [ボードマネージャー] を開き esp8266 を検索し、プラットフォームをインストールします。
4. [ツール] > [ボード] から ESP8266 の開発ボードを選択します。

開発ボードの選択は、例えば「WeMos D1 mini」を使用するのであれば「LOLIN(WEMOS) D1 R2 & mini」を選択します。

<p align="right">(<a href="#readme-top">back to top</a>)</p>

<!-- USAGE EXAMPLES -->

# 4. 使い方

WeMos D1 Mini を使ったスケッチを置いています。各プロジェクトにて詳細確認ください。

- TANK-CONTROLLER : モータドライバー(Mini L298)を使ったラジコンプロジェクト
- ROBOT-CONTROLLER : モータドライバー(Mini L298)と、超音波距離センサー(HC-SR04)を使ったロボットカープロジェクト

<p align="right">(<a href="#readme-top">back to top</a>)</p>

<!-- LICENSE -->

# 5. ライセンス

Distributed under the MIT License. See `LICENSE.txt` for more information.

<p align="right">(<a href="#readme-top">back to top</a>)</p>

<!-- ACKNOWLEDGMENTS -->

# 6. 参考

- [ESPRESSIF ESP8266](https://www.espressif.com/en/products/socs/esp8266)
- [GitHub esp8266/Arduino](https://github.com/esp8266/Arduino)
- [LOLIN D1 mini](https://www.wemos.cc/en/latest/d1/d1_mini.html)
- [Arduino 日本語リファレンス](http://www.musashinodenpa.com/arduino/ref/index.php)

## 6.1. WeMos D1 Mini ピンアウト

<img src=".//d1-mini-pinout.png" width="480">

<p align="right">(<a href="#readme-top">back to top</a>)</p>
