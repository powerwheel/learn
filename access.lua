local redis = require "resty.redis"
ip_upload_time = 60

if ngx.re.match(ngx.var.uri, 'uploadExternal') then
	local cache = redis.new()
	local ok, err = cache.connect(cache, "127.0.0.1", "6379")
	if not ok then
		ngx.log(ngx.ERR,"redis error")
		cache:close()
	else
		cache:set_timeout(6000)
		local myIP = ngx.req.get_headers()["X-Real-IP"]
		if myIP == nil then
  			 myIP = ngx.req.get_headers()["x_forwarded_for"]
		end
		if myIP == nil then
  			 myIP = ngx.var.remote_addr
		end
		local is_bind ,err = cache:get("bind_"..myIP)
		if is_bind == '1' then
			cache:close()
			return ngx.exit(403)
		else
			cache:set("bind_"..myIP, '1')
			cache:expire("bind_"..myIP, ip_upload_time)
			ngx.exec("@client")
		end			
        	cache:close()
	end
else
	ngx.exec("@client")
end
