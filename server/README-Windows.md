# Windows 原生编译与运行

Server 端现在可以使用 MSYS2 的 MinGW-w64 UCRT64 工具链编译为原生 Windows
程序。这里不使用 WSL；生成的 `gmsv.exe` 和 `saac.exe` 通过 WinSock 运行。

## 1. 安装工具链

安装 [MSYS2](https://www.msys2.org/)，打开 **MSYS2 UCRT64** 终端并执行：

```bash
pacman -Syu
pacman -S --needed mingw-w64-ucrt-x86_64-gcc \
  mingw-w64-ucrt-x86_64-cmake \
  mingw-w64-ucrt-x86_64-ninja \
  mingw-w64-ucrt-x86_64-pkgconf \
  mingw-w64-ucrt-x86_64-zlib \
  mingw-w64-ucrt-x86_64-libmariadbclient
```

本项目依赖 GNU C 扩展，因此 Windows 构建必须使用 MinGW-w64，不能使用
MSVC。SAAC 使用 MySQL C API 直连数据库，需要对应的客户端开发包。

> **注意包名**：MSYS2 仓库中已不存在 `mingw-w64-ucrt-x86_64-libmysqlclient`，
> 该开发包现在叫 `mingw-w64-ucrt-x86_64-libmariadbclient`（MariaDB
> Connector/C）。它与 MySQL C API 兼容，并且仍然提供本项目所需的两样东西：
> `ucrt64/lib/pkgconfig/mysqlclient.pc` 和 `ucrt64/lib/libmysqlclient.dll.a`，
> 因此 `CMakeLists.txt` 里的 `pkg_check_modules(MYSQL ... mysqlclient)` 与
> 回退分支 `-lmysqlclient` 都能正常工作，无需改动构建脚本。
>
> `pkgconf` 与 `zlib` 建议显式安装：`pkgconf` 让 CMake 能通过 `.pc` 文件定位
> MySQL 客户端，`zlib` 提供 `zlib1.dll`（构建后会复制到 `bin` 目录）。

如果 `pacman` 下载缓慢，可以把国内镜像放到 `/etc/pacman.d/mirrorlist.mingw`
和 `mirrorlist.msys` 的最前面：

```
Server = https://mirrors.tuna.tsinghua.edu.cn/msys2/mingw/$repo/
Server = https://mirrors.tuna.tsinghua.edu.cn/msys2/msys/$arch/
```

MSYS2 安装完成后，密钥环需要在 MSYS2 终端里初始化一次（安装器通常会做，
手动安装或安装中断时需要自己补）：

```bash
pacman-key --init
pacman-key --populate msys2
```

未完成这一步时，`/etc/pacman.conf` 中的 `SigLevel = Required` 会导致所有
`pacman -S` 失败并提示签名无效。

## 2. 编译

在 PowerShell 中从仓库根目录执行：

```powershell
powershell.exe -NoProfile -ExecutionPolicy Bypass `
  -File .\server\scripts\build-windows.ps1 -Configuration Release
```

如果 MSYS2 没有安装在 `C:\msys64`，先设置安装路径：

```powershell
$env:MSYS2_ROOT = 'D:\tools\msys64'
powershell.exe -NoProfile -ExecutionPolicy Bypass `
  -File .\server\scripts\build-windows.ps1
```

输出位于 `server\build\windows\bin`。构建会把 `zlib1.dll` 一并复制到该目录，
部署时请将它与 `gmsv.exe` 放在同一目录。

## 3. 准备运行数据

运行目录必须同时包含服务端资源与配置。至少需要：

- `acserv.cf`（SAAC 配置）；
- `setup.cf`（GMSV 配置）；
- `setup.cf` 引用的 `data/map`、`data/npc`、物品、魔法和敌人数据；
- SAAC 使用的 `char`、`char_sleep`、`data`、`lock`、`log` 和 `mail` 目录。

仓库的 `db` 目录仅包含配置和数据库初始化文件，不包含完整游戏数据。不要把
它误当成完整运行包。Windows 路径建议在配置中统一使用 `/`，例如
`D:/stoneage/runtime/data/map`；相对路径会以运行数据目录为基准。

SAAC 可在 MySQL 和内嵌 SQLite 之间切换。在 `acserv.cf` 中设置
`sql_backend mysql`（默认）或 `sql_backend sqlite`。SQLite 模式还需设置
`sql_sqlite_path saac.sqlite3`，并先用 `db/database.sqlite.sql` 初始化数据库。

同一个 SAAC 可执行文件始终包含两个后端；修改 `acserv.cf` 后重启 SAAC
即可切换，不需要重新构建。构建时仍需 MySQL 客户端开发库，运行 SQLite
模式时也需确保 MySQL 客户端运行库可被加载。SQLite 3.51.2 amalgamation
已直接编译进 SAAC。

MySQL 模式的连接参数如下：

```
sql_IP          127.0.0.1
sql_Port        3306
sql_ID          root
sql_PS          your_password
sql_DataBase    CSA
sql_Table       CSAlogin
sql_NAME        Name
sql_PASS        PassWord
sql_LOCK        LOCK
AutoReg         0
openbackground  1
```

确保 MySQL 服务已启动并可连接。

## 4. 启动

同时启动 SAAC 和 GMSV：

```powershell
powershell.exe -NoProfile -ExecutionPolicy Bypass `
  -File .\server\scripts\run-windows.ps1 -Server all `
  -DataDirectory 'D:\stoneage\runtime'
```

也可以用 `-Server saac` 或 `-Server gmsv` 单独启动。GMSV 在前台运行；按
Ctrl+C 时会执行原有的存档和关闭逻辑。`all` 模式会先在后台启动 SAAC，GMSV
退出后再关闭 SAAC。

## Windows 兼容实现说明

- WinSock 句柄不能像 POSIX 文件描述符一样直接作为连接数组下标。兼容层会将
  WinSock `SOCKET` 映射为小整数描述符，避免 64 位句柄截断和数组越界。
- `select`、非阻塞 I/O、`read`/`write`/`close` 均通过该映射转换。
- Linux 专用 epoll 模式在 Windows 下关闭，使用项目原有的 select 网络循环。
- POSIX 目录、计时、线程互斥量、进程优先级及控制台关闭信号均有 Windows
  对应实现。
