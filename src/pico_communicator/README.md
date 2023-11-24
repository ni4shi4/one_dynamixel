host向けのpico-sdkにいくつかの関数が定義されていなかったので、`harware/uart.h`をオーバーラップする形で実装(バグの発生箇所が増えるが許容する)

定義されていなかった関数
- uart_set_fifo_enabled
- uart_is_readable_within_us
