# 骑宠形象合成工具（ride_sprite_generator）

指定 A 角色形象、B 宠物形象、C 骑宠形象（C 是"某角色骑 B"的现有骑乘造型），
自动把 A 的骑手形象移植到 C 的骑乘帧上，生成"A 骑 B"的全新形象，
打包进客户端数据并注册到服务端骑宠表。

## 原理一句话

游戏把骑乘帧存成"拍扁"的索引色图。工具找到 A 已有的另一个骑乘造型（A 骑 D），
用像素匹配把 D 坐骑对掉、抠出 A 的骑手，再按 C（骑 B）每帧的骑手区域把 A 移植过去。
坐骑对齐置信度（`a_match`/`b_match`，0~1）会逐帧给出，低分帧需要手工修图。

## 文件

| 文件 | 作用 |
|---|---|
| `sprite_data.py` | 读客户端 spr/adrn/real/pal 素材库 |
| `png_codec.py` | 无依赖的索引色 PNG 读写（帧以 PNG 导出供人工修图） |
| `generator.py` | 第 1~2 步：预览单帧 / 导出整段动画工程 |
| `pack.py` | 第 3 步：把修好的 PNG 帧打包成新 GraphNo 的客户端数据 |
| `register.py` | 第 4 步：向服务端 `ridePetTable` 注册骑乘关系 |
| `test_pack.py` | pack 回读冒烟测试（合成数据，不碰真实客户端） |

## 使用流程

约定：`CLIENT=D:\workplace\stoneage\build\VER25_RELEASE`（含 `data/` 的客户端目录）。
全部命令在 `tools/ride_sprite_generator/` 下用 Python 3.10+ 运行，无第三方依赖。

### 第 1 步：单帧预览（快速看效果）

```bash
python generator.py --client %CLIENT% --a 100230 --b 100904 --c 100445 \
    --output preview --preview-only
```

- `--a` A 角色形象编号；`--b` B 宠物形象编号；`--c` C 骑乘形象编号（必须是"骑 B"的现有造型）。
- 工具自动从 `server/gmsv/char/char_base.c` 的 `ridePetTable` 找 A 的已有骑乘参照和 C 的骑手。
- 扫描 C 的全部动画，挑对齐分最高的一帧输出 `preview_A_B_C.png`（透明底）。
- 分低或抠图错位时，用 `--a-box X0 Y0 X1 Y1` / `--c-box X0 Y0 X1 Y1` 手动指定骑手区域再试。

### 第 2 步：导出整段动画工程

```bash
python generator.py --client %CLIENT% --a 100230 --b 100904 --c 100445 --output myride
```

产出 `myride/`：`frames/*.png`（逐帧图）、`manifest.json`（帧偏移/音效/质量分）、
`preview.html`（浏览器逐帧/播放检查）。**在 `frames/` 里手工修图**（保持索引色、
透明背景），刷新 preview.html 复查。`warning` 非空的帧重点检查。
试跑可加 `--max-animations 2`。

### 第 3 步：打包成新形象编号

```bash
python pack.py --project myride --client %CLIENT% --output packed_client --graphno 104032
```

- 把客户端 `data/` 的 4 个 bin + `pal/` 完整复制到 `packed_client/data/` 后追加新帧，
  **绝不原地改原客户端**。`--graphno` 省略时自动取未用编号。
- 产出 `packed_client/generated.json` 记录新编号与统计。
- 启用方式：把测试客户端指向 `packed_client`，或备份后把 4 个 bin 覆盖回原客户端
  （`bin/data` 无 git 兜底，**先备份**）。

### 第 4 步：注册到服务端骑宠表

```bash
python register.py --ride 104032 --a 100230 --b 100904 --pet-id 1642 --comment heiji --write
```

- 默认 dry-run 只打印改动；`--write` 才落盘（先备份 `char_base.c.bak`，CRLF 安全）。
- `--ride` 必须等于第 3 步的 graphno。写入 `ridePetTable` 一行 `{ride, A, B, petId}`，
  表满时自动扩容声明。写完**重新编译 gmsv** 生效。
- `--pet-id` 是 `data/enemy1.txt` 里的宠物 id（即 GM `petmake` 用的 ENEMY_ID）。
  省略时工具尝试用 B 反查（B → enemybase1.txt 第 37 列 → tempno → enemy1 id）；
  B 是骑宠专用形象（如 100904 黑鸡）反查不到时，用名字搜候选再显式指定：

  ```bash
  python register.py --find-pet 瑞里西尔
  ```

- petId 只用于"发放骑宠"流程（GM 指令/NPC 奖励），骑乘判定本身只看 `{rideNo, charNo, petNo}`。

## 注意

- 形象编号合法性由工具校验（100000~139999 且未占用）；C 必须是"骑 B"的造型，
  否则 B 坐骑对不齐，分数会很低。
- 合成图是近似结果，**逐帧人工修图是流程的一部分**，别跳过第 2 步的 preview.html 检查。
- 改服务端后别忘了构建验证四判据（obj mtime / .ninja_log / 符号 grep），别信退出码。
