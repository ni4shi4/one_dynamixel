# 初回設定

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

# 2回目以降の設定

venv環境を有効化
```console
. .venv/bin/activate
```

venv環境を無効化
```console
deactivate
```
