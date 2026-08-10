# Windows 原生编译与运行

Server 端现在可以使用 MSYS2 的 MinGW-w64 UCRT64 工具链编译为原生 Windows
程序。这里不使用 WSL；生成的 `gmsv.exe` 和 `saac.exe` 通过 WinSock 运行。

## 1. 安装工具链

安装 [MSYS2](https://www.msys2.org/)，打开 **MSYS2 UCRT64** 终端并执行：

```bash
pacman -Syu
pacman -S --needed mingw-w64-ucrt-x86_64-gcc \
  mingw-w64-ucrt-x86_64-cmake \
  mingw-w64-ucrt-x86_64-ninja
```

本项目依赖 GNU C 扩展，因此 Windows 构建必须使用 MinGW-w64，不能使用
MSVC。SAAC 的可选数据库认证在 Windows 下使用系统 ODBC 管理器；链接库由
Windows SDK 提供，无需另外下载 MySQL 开发包。

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

若启用 MSSQL/ODBC，在 Windows 的“ODBC 数据源(64 位)”中创建 DSN，并在
`acserv.cf` 中提供 `USEMSSQL`、`SQL_DSN` 等参数。不启用时设置
`USEMSSQL 0`。

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
