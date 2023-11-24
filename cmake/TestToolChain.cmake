# runするときは、pico向けのコンパイラを指定する
set(CMAKE_C_COMPILER gcc)
set(CMAKE_CXX_COMPILER g++)
set(CMAKE_ASM_COMPILER gcc)

# hostマシン向けにコンパイルできるようにする（別の場所で設定したキャッシュ変数を上書きする必要がある）
# 具体的には、hardware_...などのハードウェアとやり取りが必要なライブラリをコンパイルしないようにしている
set(PICO_PLATFORM host)
