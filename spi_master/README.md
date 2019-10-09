# SPI master convenience library for PSoC5LP

## About

PSoC5LPのSPIをマスタモードで利用する場合のサンプル。
SPIインターフェース１つのみで使う版と、複数個使う版の２種類を実装してある。

 - spi_m.h spi_m.c　SPIを一つのみ使うシングルバージョン
 - spi_m2.h spi_m2.c SPIを複数個使うマルチバージョン

## 使い方　—　SPIシングルバージョン

### ファイルの設置

- PSoC Creator のプロジェクトディレクトリへ、spi_m.h spi_m.c をコピーする。
- Project > Existing Item によりファイルダイアログを表示し、ファイルツリーをたどって spi_m.c を選ぶ。

### PSoC Creator の設定

- Communication > SPI > SPI Master Full Duplex mode Macro を配置する
- Configure ダイアログを開く
- Name が、SPIM_1 であることを確認する
- Mode, Shift Direction, Bit Rate を設定する
- 同ダイアログの Advanced タブを開く
- 以下の4箇所のチェックボックスをONにする
  - Enable Tx Internal Interrupt
  - Interrupt On Byte/Word Transfer Complete
  - Enable Rx Internal Interrupt
  - Interrupt On Rx FIFO Not Empty.
- OKボタンでダイアログを確定する
- Ports and Pins > Digital Output Pin を配置する
- Configure ダイアログを開く
- Name を SS に変更する
- OKボタンでダイアログを確定する
- SPIM_1 の ss と 接続する
- cyapicallbacks.h ファイルが生成されているので、以下の通り追記する。
```
#define SPIM_1_TX_ISR_ENTRY_CALLBACK
void SPIM_1_TX_ISR_EntryCallback(void);
#define SPIM_1_RX_ISR_ENTRY_CALLBACK
void SPIM_1_RX_ISR_EntryCallback(void);
```

### ライブラリの利用

初期化
```
#include "spi_m.h"

int main(void)
{
  CyGlobalIntEnable; /* Enable global interrupts. */

  spi_init();
```

送受信例
```
  uint8_t send[] = { 0x2c, 0x0a };
  uint8_t recv[10];

  // 2バイト送信に続いて10バイト受信
  spi_transfer(send, 2, recv, 10, 0);

　// 2バイト送信のみ
  spi_transfer(send, 2, 0, 0, 0);

  // 10バイト受信のみ
  spi_transfer(0, 0, recv, 10, 0);
```

いずれも、割り込みによりバックグラウンドで動作する。必要に応じ、以下の関数により同期させる。

```
spi_wait_done();
spi_is_transfer();
```


## 使い方　—　SPIマルチバージョン

### ファイルの設置

- PSoC Creator のプロジェクトディレクトリへ、spi_m2.h spi_m2.c をコピーする。
- Project > Existing Item によりファイルダイアログを表示し、ファイルツリーをたどって spi_m2.c を選ぶ。

### PSoC Creator の設定

シングルバージョンと同じ手順で、複数個のインタフェースを配置する。
その場合、SPIM_1, SPIM_2,... の順番で命名されることを確認する。

cyapicallbacks.h ファイルへは、インターフェースの個数分の記述を行う。
```
#define SPIM_1_TX_ISR_ENTRY_CALLBACK
void SPIM_1_TX_ISR_EntryCallback(void);
#define SPIM_1_RX_ISR_ENTRY_CALLBACK
void SPIM_1_RX_ISR_EntryCallback(void);

#define SPIM_2_TX_ISR_ENTRY_CALLBACK
void SPIM_2_TX_ISR_EntryCallback(void);
#define SPIM_2_RX_ISR_ENTRY_CALLBACK
void SPIM_2_RX_ISR_EntryCallback(void);
```

### ライブラリの利用

初期化
```
#include "spi_m2.h"

SPI_HANDLE spih1;
SPI_HANDLE spih2;
SPI_ISR( &spih1, SPIM_1 );
SPI_ISR( &spih2, SPIM_2 );

int main(void)
{
  CyGlobalIntEnable; /* Enable global interrupts. */

  spi_init( &spih1, SPIM_1 );
  spi_init( &spih2, SPIM_2 );
```

送受信例
```
  uint8_t send[] = { 0x2c, 0x0a };
  uint8_t recv[10];

  // 2バイト送信に続いて10バイト受信
  spi_transfer(&spih1, send, 2, recv, 10, 0);

　// 2バイト送信のみ
  spi_transfer(&spih1, send, 2, 0, 0, 0);

  // 10バイト受信のみ
  spi_transfer(&spih1, 0, 0, recv, 10, 0);
```

いずれも、割り込みによりバックグラウンドで動作する。必要に応じ、以下の関数により同期させる。

```
spi_wait_done(&spih1);
spi_is_transfer(&spih1);
```
