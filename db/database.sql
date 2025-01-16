SET SQL_MODE="NO_AUTO_VALUE_ON_ZERO";
SET time_zone = "+00:00";

-- 数据库: `stoneage(sa)`
CREATE DATABASE IF NOT EXISTS sa;
USE sa;

-- 表的结构 `capture_pet`
CREATE TABLE IF NOT EXISTS `sa.capture_pet` (
  `unicode` varchar(32) NOT NULL,
  `id` int(11) NOT NULL,
  `name` varchar(32) CHARACTER SET utf8mb4 NOT NULL,
  `type` int(11) NOT NULL,
  `lv` int(11) NOT NULL,
  `hp` int(11) NOT NULL,
  `attack` int(11) NOT NULL,
  `def` int(11) NOT NULL,
  `quick` int(11) NOT NULL,
  `sum` double NOT NULL,
  `author` varchar(32) CHARACTER SET utf8mb4 NOT NULL,
  `cdkey` varchar(32) CHARACTER SET utf8mb4 NOT NULL,
  `check` tinyint(1) NOT NULL DEFAULT '0',
  `created_time` timestamp NOT NULL,
  PRIMARY KEY (`unicode`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8mb4;

-- 表的结构 `cash_draw`
CREATE TABLE IF NOT EXISTS `sa.cash_draw` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `type` varchar(16) NOT NULL,
  `point` int(11) NOT NULL,
  `fixtime` datetime NOT NULL,
  PRIMARY KEY (`id`),
  UNIQUE KEY `id` (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- 表的结构 `check`
CREATE TABLE IF NOT EXISTS `sa.check` (
  `itemcode` varchar(64) NOT NULL,
  `cdkey` varchar(32) NOT NULL,
  `name` varchar(32) CHARACTER SET utf8mb4 NOT NULL,
  `mac` varchar(64) NOT NULL,
  `type` varchar(32) CHARACTER SET utf8mb4 NOT NULL,
  `value` int(11) NOT NULL,
  `time` datetime NOT NULL,
  `check` tinyint(4) NOT NULL DEFAULT '0',
  `usecdkey` varchar(32) DEFAULT NULL,
  `usename` varchar(32) CHARACTER SET utf8mb4 DEFAULT NULL,
  `usemac` varchar(64) DEFAULT NULL,
  `usetime` datetime DEFAULT NULL,
  PRIMARY KEY (`itemcode`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8mb4;

-- 表的结构 `consignment` 委托
CREATE TABLE IF NOT EXISTS `sa.consignment` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `cdkey` varchar(32) COLLATE utf8mb4_unicode_ci NOT NULL,
  `name` varchar(32) COLLATE utf8mb4_unicode_ci NOT NULL,
  `alive` int(11) NOT NULL,
  `vippoint` int(11) NOT NULL,
  `time` datetime NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

-- 表的结构 `counter`
CREATE TABLE IF NOT EXISTS `sa.counter` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `cdkey` varchar(32) NOT NULL,
  `name` varchar(32) CHARACTER SET utf8mb4 NOT NULL,
  `type` int(11) NOT NULL,
  `vippoint` int(11) NOT NULL,
  `alive` int(11) NOT NULL,
  `fame` int(11) NOT NULL,
  `fixtime` datetime NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- 表的结构 `CSAinfo`
CREATE TABLE IF NOT EXISTS `sa.CSAinfo` (
  `cdkey` varchar(32) CHARACTER SET utf8mb4 NOT NULL,
  `saveindex` int(11) NOT NULL,
  `name` varchar(32) CHARACTER SET utf8mb4 NOT NULL,
  `famename` varchar(32) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NOT NULL,
  `CE` int(16) NOT NULL DEFAULT '0',
  `level` int(11) NOT NULL,
  `trans` int(11) NOT NULL,
  `alive` int(11) NOT NULL,
  `cameo` int(11) NOT NULL,
  `fame` int(11) NOT NULL,
  `TITLE_LV` int(11) NOT NULL,
  `DUELBATTLECOUNT` int(11) NOT NULL,
  `DUELWINCOUNT` int(11) NOT NULL,
  `DUELLOSECOUNT` int(11) NOT NULL,
  `DUELSTWINCOUNT` int(11) NOT NULL,
  `DUELSTLOSECOUNT` int(11) NOT NULL,
  `DUELMAXSTWINCOUNT` int(11) NOT NULL,
  `RESETPETCOUNT` int(16) NOT NULL DEFAULT '0' COMMENT '洗宠次数',
  `AWAKEPETCOUNT` int(16) NOT NULL DEFAULT '0' COMMENT '变异次数',
  `SEALEVNTCOUNT` int(16) NOT NULL DEFAULT '0' COMMENT '任务封印次数',
  `TRANSPETCOUNT` int(16) NOT NULL DEFAULT '0' COMMENT '转宠次数',
  `KILLBOSSCOUNT` int(16) NOT NULL DEFAULT '0' COMMENT '击杀BOSS次数',
  `BOSSKILLCOUNT` int(16) NOT NULL DEFAULT '0' COMMENT '被BOSS杀次数',
  `CAPPETCOUNT` int(16) NOT NULL DEFAULT '0' COMMENT '抓宠次数',
  `FEEDMMCOUNT` int(16) NOT NULL DEFAULT '0' COMMENT '练MM次数',
  `EQUIPUPCOUNT` int(16) NOT NULL DEFAULT '0' COMMENT '强化成功次数',
  `EQUIPFAILCOUNT` int(16) NOT NULL DEFAULT '0' COMMENT '强化失败次数',
  `MAGICADDCOUNT` int(16) NOT NULL DEFAULT '0' COMMENT '附魔次数',
  `HORSEWINCOUNT` int(16) NOT NULL DEFAULT '0' COMMENT '跑马押中次数',
  `HUNTFISHCOUNT` int(16) NOT NULL DEFAULT '0' COMMENT '捕鱼次数',
  `HUNTBIGFISHCOUNT` int(16) NOT NULL DEFAULT '0' COMMENT '大鱼次数',
  `vip` int(11) NOT NULL,
  `time` datetime NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

-- 表的结构 `CSAlogin`
CREATE TABLE IF NOT EXISTS `sa.CSAlogin` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `username` varchar(32) CHARACTER SET utf8mb4 NOT NULL,
  `password` varchar(32) CHARACTER SET utf8mb4 NOT NULL,
  `IP` varchar(16) NOT NULL DEFAULT '-',
  `MAC` varchar(64) NOT NULL,
  `RegTime` datetime NOT NULL DEFAULT '0000-00-00 00:00:00',
  `LoginTime` datetime DEFAULT '0000-00-00 00:00:00',
  `OnlineName` varchar(32) CHARACTER SET utf8mb4 DEFAULT NULL,
  `OnlineName1` varchar(30) CHARACTER SET utf8mb4 DEFAULT NULL,
  `OnlineName2` varchar(32) CHARACTER SET utf8mb4 DEFAULT NULL,
  `Online` tinyint(4) DEFAULT '0',
  `Path` varchar(16) DEFAULT '',
  `CE` int(16) NOT NULL DEFAULT '0',
  `VipPoint` int(11) DEFAULT '0',
  `PayPoint` int(11) NOT NULL,
  `GamblePoint` int(11) NOT NULL,
  `TrialPoint` int(16) NOT NULL DEFAULT '0',
  `GoldPoint` int(16) NOT NULL DEFAULT '0',
  `SQLPoint` int(16) NOT NULL DEFAULT '0',
  `SafePasswd` varchar(32) DEFAULT '',
  `ServerId` int(4) NOT NULL DEFAULT '0',
  `Regip` varchar(32) NOT NULL,
  `qq` varchar(32) NOT NULL,
  `tel` varchar(32) NOT NULL,
  `ServerName` varchar(32) NOT NULL,
  `Recommender` int(11) NOT NULL,
  `Lock` int(4) NOT NULL DEFAULT '0',
  `LockTime` datetime NOT NULL DEFAULT '0000-00-00 00:00:00',
  `LockInfo` varchar(32) NOT NULL DEFAULT '',
  `Offline` tinyint(4) NOT NULL DEFAULT '0',
  `Trust` tinyint(4) NOT NULL DEFAULT '0',
  PRIMARY KEY (`id`),
  UNIQUE KEY `username` (`username`)
) ENGINE=MyISAM  DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

-- 表的结构 `dice`
CREATE TABLE IF NOT EXISTS `sa.dice` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `cdkey` varchar(32) NOT NULL,
  `name` varchar(32) CHARACTER SET utf8mb4 NOT NULL,
  `type` int(11) NOT NULL,
  `vippoint` int(11) NOT NULL,
  `alive` int(11) NOT NULL,
  `fame` int(11) NOT NULL,
  `dicenum` int(11) NOT NULL,
  `fixtime` datetime NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- 表的结构 `dicelog`
CREATE TABLE IF NOT EXISTS `sa.dicelog` (
  `type` int(11) NOT NULL DEFAULT '0',
  `winpoint` int(11) NOT NULL DEFAULT '0',
  `losepoint` int(11) NOT NULL DEFAULT '0',
  PRIMARY KEY (`type`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8mb4;

-- 表的结构 `loan`
CREATE TABLE IF NOT EXISTS `sa.loan` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `cdkey` varchar(32) CHARACTER SET utf8mb4 NOT NULL,
  `name` varchar(32) CHARACTER SET utf8mb4 NOT NULL,
  `PetName` varchar(32) CHARACTER SET utf8mb4 NOT NULL,
  `PetInfo` varchar(512) CHARACTER SET utf8mb4 NOT NULL,
  `PetString` varchar(2048) CHARACTER SET utf8mb4 NOT NULL,
  `day` int(11) NOT NULL,
  `vippoint` int(11) NOT NULL,
  `stat` int(11) NOT NULL DEFAULT '0',
  `loancdkey` varchar(32) CHARACTER SET utf8mb4 NOT NULL,
  `loanname` varchar(32) CHARACTER SET utf8mb4 NOT NULL,
  `fixtime` datetime NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- sa.lock: 用于记录锁定的用户.
CREATE TABLE IF NOT EXISTS `sa.lock` (
  `Name` varchar(64) NOT NULL,
  `LockTime` datetime NOT NULL,
  PRIMARY KEY (`Name`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8mb4;

-- 表的结构 `OnlineBuy`
CREATE TABLE IF NOT EXISTS `sa.OnlineBuy` (
  `CostPasswd` varchar(32) NOT NULL,
  `CostStr` varchar(64) DEFAULT NULL,
  `cdkey` varchar(32) DEFAULT NULL,
  `CostTime` datetime DEFAULT NULL,
  `check` tinyint(1) DEFAULT '1',
  PRIMARY KEY (`CostPasswd`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8mb4;

-- 表的结构 `OnlineCard`
CREATE TABLE IF NOT EXISTS `sa.OnlineCard` (
  `CostPasswd` varchar(32) NOT NULL,
  `CostStr` varchar(64) DEFAULT NULL,
  `cdkey` varchar(32) DEFAULT NULL,
  `CostTime` datetime DEFAULT NULL,
  `check` tinyint(1) DEFAULT '1',
  PRIMARY KEY (`CostPasswd`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8mb4;

-- 表的结构 `OnlineCost`
CREATE TABLE IF NOT EXISTS `sa.OnlineCost` (
  `CostPasswd` varchar(32) NOT NULL,
  `CostVal` int(11) NOT NULL,
  `PayVal` int(11) NOT NULL,
  `cdkey` varchar(32) DEFAULT '',
  `CostTime` datetime NOT NULL,
  `check` tinyint(1) NOT NULL DEFAULT '1',
  `creator` varchar(32) DEFAULT NULL,
  UNIQUE KEY `CostPasswd` (`CostPasswd`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8mb4;

-- 表的结构 `OnlinePlayer`
CREATE TABLE IF NOT EXISTS `sa.OnlinePlayer` (
  `cdkey` varchar(32) NOT NULL,
  `MAC` varchar(64) NOT NULL,
  `Name` varchar(32) NOT NULL,
  `LoginTime` datetime NOT NULL,
  `ServerId` int(11) NOT NULL,
  PRIMARY KEY (`cdkey`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- 表的结构 `PauctionInfo`
CREATE TABLE IF NOT EXISTS `sa.PauctionInfo` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `cdkey` varchar(32) CHARACTER SET utf8mb4 NOT NULL,
  `name` varchar(32) CHARACTER SET utf8mb4 NOT NULL,
  `effect` varchar(1024) CHARACTER SET utf8mb4 NOT NULL,
  `cost` int(11) NOT NULL,
  `type` int(11) NOT NULL,
  `info` varchar(1024) CHARACTER SET utf8mb4 NOT NULL,
  `string` varchar(1024) CHARACTER SET utf8mb4 NOT NULL,
  `day` datetime NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- 表的结构 `petbilling`
CREATE TABLE IF NOT EXISTS `sa.petbilling` (
  `id` int(11) NOT NULL,
  `type` tinyint(4) NOT NULL,
  `name` varchar(32) CHARACTER SET utf8mb4 NOT NULL,
  `lv` int(11) NOT NULL,
  `hp` int(11) NOT NULL,
  `attack` int(11) NOT NULL,
  `def` int(11) NOT NULL,
  `quick` int(11) NOT NULL,
  `author` varchar(32) CHARACTER SET utf8mb4 NOT NULL,
  PRIMARY KEY (`id`,`type`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8mb4;

-- 表的结构 `pklist`
CREATE TABLE IF NOT EXISTS `sa.pklist` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `player1` varchar(32) CHARACTER SET utf8mb4 NOT NULL,
  `player2` varchar(32) CHARACTER SET utf8mb4 NOT NULL,
  `player3` varchar(32) CHARACTER SET utf8mb4 NOT NULL,
  `player4` varchar(32) CHARACTER SET utf8mb4 NOT NULL,
  `player5` varchar(32) CHARACTER SET utf8mb4 NOT NULL,
  `seesion` int(11) NOT NULL,
  `stat` int(11) NOT NULL DEFAULT '0',
  UNIQUE KEY `id` (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- 表的结构 `safedata`
CREATE TABLE IF NOT EXISTS `sa.safedata` (
  `cdkey` varchar(32) NOT NULL DEFAULT '',
  `newsafe` varchar(64) DEFAULT NULL,
  `oldsafe` varchar(64) DEFAULT NULL,
  `flg` int(1) NOT NULL,
  `FixTime` datetime NOT NULL,
  PRIMARY KEY (`cdkey`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8mb4;

-- 表的结构 `SaleVipPoint`
CREATE TABLE IF NOT EXISTS `sa.SaleVipPoint` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `cdkey` varchar(32) NOT NULL,
  `name` varchar(32) CHARACTER SET utf8mb4 NOT NULL,
  `Vippoint` int(11) NOT NULL,
  `rate` int(11) NOT NULL,
  `price` int(11) NOT NULL,
  `stat` int(11) NOT NULL,
  `CreateTime` datetime NOT NULL,
  `PayTime` datetime NOT NULL,
  `banktype` varchar(32) CHARACTER SET utf8mb4 NOT NULL DEFAULT '',
  `bankcdkey` varchar(32) CHARACTER SET utf8mb4 NOT NULL,
  `bankname` varchar(8) CHARACTER SET utf8mb4 NOT NULL,
  `QQ` varchar(11) CHARACTER SET utf8mb4 NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- 表的结构 `SuperStar`
CREATE TABLE IF NOT EXISTS `sa.SuperStar` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `cdkey` varchar(32) CHARACTER SET utf8mb4 NOT NULL,
  `name` varchar(32) CHARACTER SET utf8mb4 NOT NULL,
  `award` varchar(32) CHARACTER SET utf8mb4 NOT NULL,
  `check` int(11) NOT NULL,
  `datatime` datetime NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- 表的结构 `transpointlog`
CREATE TABLE IF NOT EXISTS `sa.transpointlog` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `incdkey` varchar(32) DEFAULT NULL,
  `outcdkey` varchar(32) DEFAULT NULL,
  `type` varchar(32) DEFAULT NULL,
  `inpoint` int(11) DEFAULT '0',
  `outpoint` int(11) DEFAULT '0',
  `datetime` datetime DEFAULT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- 表的结构 `VipShop`
CREATE TABLE IF NOT EXISTS `sa.VipShop` (
  `cdkey` varchar(32) COLLATE utf8mb4_unicode_ci NOT NULL,
  `name` varchar(32) CHARACTER SET utf8mb4 NOT NULL,
  `itemid` int(11) NOT NULL,
  `itemname` varchar(32) CHARACTER SET utf8mb4 NOT NULL,
  `itemnum` int(11) NOT NULL,
  `totalcost` int(16) NOT NULL,
  `time` datetime NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

-- 表的结构 `weekpay`
CREATE TABLE IF NOT EXISTS `sa.weekpay` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `term` int(11) NOT NULL,
  `cdkey` varchar(32) NOT NULL,
  `itemid` int(11) NOT NULL,
  `check` int(11) NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- 表的结构 `YamaKing`
CREATE TABLE IF NOT EXISTS `sa.YamaKing` (
  `index` int(13) NOT NULL AUTO_INCREMENT,
  `cdkey` varchar(32) CHARACTER SET utf8mb4 NOT NULL,
  `name` varchar(32) CHARACTER SET utf8mb4 NOT NULL,
  `floor` int(16) NOT NULL,
  `itemid` int(16) NOT NULL,
  `itemname` varchar(16) CHARACTER SET utf8mb4 NOT NULL,
  `time` datetime NOT NULL,
  `mac` varchar(64) CHARACTER SET utf8mb4 NOT NULL,
  `servername` varchar(32) CHARACTER SET utf8mb4 NOT NULL,
  PRIMARY KEY (`index`)
) ENGINE=InnoDB  DEFAULT CHARSET=utf8mb4;

-- 表的结构 `yuxiaxie`
CREATE TABLE IF NOT EXISTS `sa.yuxiaxie` (
  `date` date NOT NULL,
  `cdkey` varchar(32) CHARACTER SET utf8mb4 NOT NULL,
  `name` varchar(32) CHARACTER SET utf8mb4 NOT NULL,
  `type` int(11) NOT NULL,
  `vippoint` int(11) NOT NULL,
  `fame` int(11) NOT NULL,
  `alive` int(11) NOT NULL,
  `fixtime` datetime NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- 表的结构 `yuxiaxielog`
CREATE TABLE IF NOT EXISTS `sa.yuxiaxielog` (
  `date` date NOT NULL,
  `type` int(11) NOT NULL,
  `vippoint` int(11) NOT NULL,
  `alive` int(11) NOT NULL,
  `fame` int(11) NOT NULL,
  `sumvippoint` int(11) NOT NULL,
  `sumalive` int(11) NOT NULL,
  `sumfame` int(11) NOT NULL,
  `fixtime` datetime NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

