local string = require "string"
local json = require "cjson.safe"
---local gray_list={['1B'] = 1,['1A'] = 1, ['08'] = 1, ['2A'] = 1}
local gray_percent = 5 --%
local ua = ngx.req.get_headers()["User-Agent"]
if ua == nil then
    ngx.say('user agent invaild!')
    return ngx.HTTP_OK
end
ngx.log(ngx.INFO, ua)
local isWindows = true
local info, err = ngx.re.match(ua, "(Windows)")
if not info then
    isWindows = false
    ngx.exec("@mobile")
end
local version, device_id
local info, err = ngx.re.match(ua, "([0-9]{1}.[0-9]{1}.[0-9]{1})")
if info then
    version = info[0]
else
    ngx.say("can't find version")
    return ngx.HTTP_OK
end

local info, err = ngx.re.match(ua, "([0-9A-Fa-f]{8}-[0-9A-Fa-f]{4}-[0-9A-Fa-f]{4}-[0-9A-Fa-f]{4}-[0-9A-Fa-f]{12})")
if info then
    device_id = info[0]
else
    ngx.say("can't find deviceId")
    return ngx.HTTP_OK
end

local major = string.sub(version, 1, 1)
local minor = string.sub(version, 3, -3)
local patch = string.sub(version, -1)


local update_type = "new"
if major <= '4' and minor <= '2' and patch <= '3' then
    update_type = "old"
end
local grayid = "0x" .. string.sub(device_id,-2)
local gray_range = 255 * gray_percent / 100
ngx.log(ngx.INFO, "gray range:", gray_range)
ngx.log(ngx.INFO, "req id:", tonumber(grayid))
if isWindows and tonumber(grayid) <= gray_range then
    ngx.log(ngx.INFO, "in gray list")
    if update_type == "old" then
        ngx.log(ngx.INFO, "old version")
        local data = {
            ['success'] = true,
            ['data'] = {
                ['client'] = {
                    ['version'] = "",
                    ['description'] = 'update',
                    ['description_full'] = '',
                    ['download_url'] = "",
                    ['download_size'] = '',
                    ['download_sha1'] = '',
                    ['force_upgrade'] = true,
                    ['force_message'] = ''
                },
                ['settings_basis'] = {
                    ['version'] = '4.1.0.0',
                    ['need_restart'] = true
                }
            }
        }
	ngx.log(ngx.ERR, ua)
        ngx.say(json.encode(data))
    else
        local bodys = ngx.req.get_body_data()
        ngx.log(ngx.INFO, "req body:", bodys)
        local body = json.decode(bodys)
	if not body then
		return ngx.HTTP_BAD_REQUEST
	end
        if body['params']['clientBody']['appName'] == '' then
            local data = {
                ['id'] = device_id,
                ['ncp'] = '2.0.0',
                ['soa'] = {
                    ['rpc'] = '2.0.0',
                    ['req'] = body['id']
                },
                ['result'] = {
                    ['versionUpdate'] = {
                        ['force'] = true,
                        ['detail'] = {
                            ['version'] = "",
                            ['description'] = '',
                            ['description_full'] = 'update',
                            ['download_url'] = "",
                            ['download_size'] = '',
                            ['download_sha1'] = '',
                            ['force_upgrade'] = true,
                            ['force_message'] = ''
                        }
                    },
		    ['settingUpdate'] = {
		    }
                },
                ['metas'] = {
                },
		['error'] = json.null
            }
            ngx.log(ngx.ERR,ua)
        --    ngx.log(ngx.ERR,"resp:", json.encode(data))
            ngx.say(json.encode(data))
        end
    end
else
    ngx.log(ngx.INFO, "not in gray list")
    if isWindows and update_type == "old" then
        local data = {
            ["success"] = true,
            ["data"] = {
                ['client'] = json.null,
                ['settings_basis'] = {
                    ["need_restart"] = true
                }
            }
        }
        ngx.say(json.encode(data))
    else
        local bodys = ngx.req.get_body_data()
        ngx.log(ngx.INFO, "req body:", bodys)
        local body = json.decode(bodys)
	local id
	if body then
		id = body['id']
	else
		id = device_id
	end
        local data = {
            ['id'] = device_id,
            ['ncp'] = '2.0.0',
            ['soa'] = {
                ['rpc'] = '2.0.0',
                ['req'] = id
            },
	    ['result'] = {
		['versionUpdate'] = {
		},
		['settingUpdate'] = {
		}
	    },
            ['metas'] = {

            },
	    ['error'] = json.null
        }
        ngx.say(json.encode(data))
    end
end
return ngx.HTTP_OK
