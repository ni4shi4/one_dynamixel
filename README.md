# 開発の進め方

## 初回設定

以下のコマンドを実行する
```console
# venvの設定
python3 -m venv .venv
# venv環境を有効化
. .venv/bin/activate
# 開発に必要なパッケージのinstall
pip install -r requirements-dev.txt
# pre-commitの有効化
pre-commit install
```

## 2回目以降の設定

venv環境を有効化
```console
. .venv/bin/activate
```

venv環境を無効化
```console
deactivate
```

## ビルド方法

- Presetにて、Picoにインストールするためのビルド環境とテスト用のビルド環境を分けている
    - テスト用のビルド環境については、[テスト方法のセクション](#テスト方法)を参照
- Presetとしては以下の2つを用意している
    - run_debug : `CMAKE_BUILD_TYPE` = `Debug`でビルド
    - run_release : `CMAKE_BUILD_TYPE` = `Release`でビルド

### 実行コマンド

ビルド用のPresetに変更
```console
cmake --preset run_debug
```

テスト用のPresetでビルド
```console
cmake --build --preset run_debug
```

## テスト方法

- ThrowTheSwitchの[Unity](https://github.com/ThrowTheSwitch/Unity.git)を使ってテストを行っている
    - デフォルトの設定に加えて、unity_fixtureのextensinoを有効にしている(テスト用のPresetで`UNITY_EXTENSION_FIXTURE`を`ON`にしている)
- テスト用のPresetを使って、pico-sdkによるコンパイラの設定を上書きするようにしている
    - Unityを使用するためには、`arm-none-eabi-gcc`の実装では不十分なため、テスト時はOSのライブラリを使用するようにしている
    - テストで確認できるのは、ロジックまでで実機を使ったテストはできない
- テスト用Presetでの追加設定
    - Unityの追加
    - コンパイラの変更
    - pico-sdkのビルドをhost向けに変更
    - pico実機が必要なソースファイルをビルド対象から外す


### 実行コマンド

テスト用のPresetに変更
```console
cmake --preset test
```

テスト用のPresetでビルド
```console
cmake --build --preset test
```

テスト用のPresetでテスト
```console
cmake --build --preset test
```

### テストコードの作成

[Unityのコード例](See https://github.com/ThrowTheSwitch/Unity/tree/master/examples/example_2 for more detail)を参照

### Workflow

Presetにworkflow用のPresetを設定すると、1コマンドでビルド・テストを実行できる
```console
cmake --workflow --preset test
```
ただし、VSCode(1.83.0)で`workflowPresets`をサポートしていないらしいので、`CMakePresets.json`に設定を追加していない

workflow用のPresetの設定
```json
"workflowPresets": [
    {
        "name": "test",
        "displayName": "Test Workflow",
        "description": "Workflow for testing code",
        "steps": [
            {
                "type": "configure",
                "name": "test"
            },
            {
                "type": "build",
                "name": "test"
            },
            {
                "type": "test",
                "name": "test"
            }
        ]
    }
]
```
