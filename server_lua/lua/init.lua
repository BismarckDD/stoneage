function loadScript(path)
	return dofile(path);
end



function main()
	loadScript("./data/lua/onlineshop/main.lua");
	--时间种子
	math.randomseed(os.time());
end

main()



