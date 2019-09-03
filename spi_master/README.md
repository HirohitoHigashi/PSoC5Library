# SPI master convenience library for PSoC5LP

## About

PSoC5LPのSPIをマスタモードで利用する場合のサンプル。
この版では、SPIインタフェースを１つのみ利用できる。

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
- Congigure ダイアログを開く
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
