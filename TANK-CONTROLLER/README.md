<a name="readme-top"></a>

<!-- ABOUT THE PROJECT -->

# 1. プロジェクトについて

WiFi が使用できるマイコンボード WeMos D1 Mini と、モータードライバー Mini L298 を使ったラジコンの Arduino IDE のスケッチです。

D1 Mini を WiFi アクセスポイント として、スマホからアクセスし、表示されたページからラジコンを操作します。
操作を受けた D1 Mini はモータードライバーを介してモーターを制御し、タイヤを自由自在に回転させます。
Arduino IDE からスケッチをマイコンボードに書き込みし、ホームページのファイルをマイコンボードをコピーすることで、スマホから操作のできるラジコンを作ることができます。

制御するモーターはなんでも良いので、様々な環境で使用することができます。

<p align="right">(<a href="#readme-top">back to top</a>)</p>

<!-- ABOUT THE TARGET -->

# 2. Mini L298 仕様

ダブルチャンネル H ブリッジモータードライブ  
同時にダブルチャンネル直流モータードライブ、または 1 個 4 ライン 2 相ステッピングモータ

- モジュール給電電圧：2V-10V
- 信号端入力電圧：1.8-7V
- シングルチャンネル作動電流：1.5A、ピーク電流：2.5A、待ち受け電流：0.1uA 以下
- 商品寸法：24.7 x 21 x 5mm(長さ、幅、高さ)
- 取り付け穴直径：2mm

アンチコモンステート導通回路付き，入力を接続してない時にモーターが誤作動なし  
ヒステリシス効果の過熱保護回路付き (TSD)、モーターがショートで壊れることは心配なし

<p align="right">(<a href="#readme-top">back to top</a>)</p>

<!-- GETTING STARTED -->

# 3. さあ、はじめよう！

本プロジェクトのスケッチを使用するには、事前に Arduino IDE 開発環境と ESP8266 が動作するための設定が必要です。

<p align="right">(<a href="#readme-top">back to top</a>)</p>

<!-- USAGE EXAMPLES -->

# 4. 使い方

## 4.1. ファイルアップロード

タンクをコントロールするためのホームページ「index.html」を D1 Mini へアップロードするには LittleFS を使用します。

- [Arduino ESP8266 LittleFS Filesystem Uploader](https://github.com/earlephilhower/arduino-esp8266littlefs-plugin)

Arduino IDE 2.0 以降は使用できないようですので、Arduino IDE 1.8 をご使用ください。

## 4.2. 配線図

<img src="./img/wiring-diagram.jpeg" width="480">

<p align="right">(<a href="#readme-top">back to top</a>)</p>

<!-- LICENSE -->

# 5. ライセンス

<p align="right">(<a href="#readme-top">back to top</a>)</p>

<!-- ACKNOWLEDGMENTS -->

# 6. 参考

- [Arduino で DC モーターを制御する](https://burariweb.info/electronic-work/arduino-learning/arduino-motor-driver-l298n.html)
- [Arduino と Mini L298N モータードライバーでモーターを制御する](https://web-memo-s.hatenablog.com/entry/2021/02/22/125619)
- [Create a WIFI-controlled Robot with Wemos D1 Mini + L298N](https://www.youtube.com/watch?v=BuSS_MBPCEk)

Google や Youtube で「D1 Mini L298N」を検索すると色々情報入手できます。

## 6.1. ラジコンカー

<img src="./img/rc-car.jpeg" width="480">

## 6.2. ラジコンタンク

<img src="./img/rc-tank.jpeg" width="480">

<p align="right">(<a href="#readme-top">back to top</a>)</p>
