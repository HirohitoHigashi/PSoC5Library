# UART wrapper for PSoC5LP

## Why created this?

- 文字列の送受信を簡単に行いたい。
- 可能な限りデッドロックを避けたい。
- 特定の条件下では、標準のライブラリのFIFOパラメータを増やして対応するよりも、省電力にできそうだ。

### 電流値ベンチマーク

CY8CKIT-059 (CY8C5888LTI-LP097) 24MHz, 5V

| | send / recv| Current(mA) |
|-|-|-|
| Full Load    |         | 18.0 |
| Cypress Lib. | Send    | 13.6 |
|              | Receive | 13.2 |
| This Lib.    | Send    | 8.0 |
|              | Receive | 7.7 |


# How to use.

## PSoC Creatorの設定

- Communication > UARTデバイス配置
 - Configureダイアログを開く
 - Nameが"UART_1"であることを確認
 - 通信速度等のパラメータ設定
 - 同ダイアログAdvancedタブのInterruptを、"RX- On Byte Received"と"TX - On FIFO Empty"にチェックを入れる
- System > Interrupt デバイスを2つ配置
 - UARTのtx_interruptとrx_interruptに接続
 - 名前をそれぞれ、"isr_UART_1_Tx"と、"isr_UART_1_Rx"に変更

## ライブラリ使用

### 初期化

標準版（UART一つのみ）
```
#include "uart.h"
UART_HANDLE uh;

int main()
{
  uart_init( &uh );
}
```

### 初期化

複数版（UART２つ以上）
```
#include "uart2.h"
UART_HANDLE uh;
UART_ISR( &uh, UART_1 );

int main()
{
  uart_init( &uh, UART_1 );
}
```


### 文字列送信

```
int txlen = uart_puts( &uh, "String" );
```

### 文字列受信

```
char buf[32];
rxlen = uart_gets( &uh, buf, sizeof(buf) );
```

※バイナリ送受信も可能、ソースコード参照。
