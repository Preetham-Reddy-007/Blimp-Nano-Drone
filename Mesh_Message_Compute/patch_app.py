import os
import json

def patch_app_js(json_file, js_file, pwd):
    data = json.load(json_file)
    
    for item in data["netKeys"]:
        net_key = item["key"]
        #print("Net Key")
        #print(net_key)
    for item in data["appKeys"]:
        app_key = item["key"]
    
    node_names = []
    node_unicast = []
    
    for item in data["nodes"]:
        node_names.append(item["name"])
        node_unicast.append(item["unicastAddress"])
    
    params = ["00000000",net_key, app_key]
    key_words = ['var iv_index = ', 'var netkey = ','var appkey = ']
    template = ['var iv_index = "{}";\n','var netkey = "{}";\n','var appkey = "{}";\n']
    lines_to_write = []
    for line in js_file:    
        for key_item in key_words:
            if key_item in line:
                _index = key_words.index(key_item)
                print("Replacing for index "+str(_index))
                line = template[_index].format(params[_index])
        lines_to_write.append(line)
    new_file = open(pwd+"\\js\\app.js","w")
    new_file.writelines(lines_to_write)
    new_file.close()
    print("JSON to app.js Patch complete!")

def patch_index(json_file, index_file):
    data = json.load(json_file)
    
    for item in data["netKeys"]:
        net_key = item["key"]
    for item in data["appKeys"]:
        app_key = item["key"]
    
    node_names = []
    node_unicast = []
    
    for item in data["nodes"]:
        node_names.append(item["name"])
        node_unicast.append(item["unicastAddress"])
    
    params = [net_key, app_key, "00000000", "000007","000007"]
    for (p,q) in zip(node_unicast,node_names):
        params.append([p,q])
    
    key_words = [   'id="netkey"','id="appkey"','id="iv_index"','id="seq"', 'incremented',\
                    '<!-- Option NODE ID 1 -->','<!-- Option NODE ID 2 -->',\
                    '<!-- Option NODE ID 3 -->','<!-- Option NODE ID 4 -->']
    template = ['<input type="text" class="arg" id="netkey" maxlength="32" size="34" value="{}">\n',\
                '<input type="text" class="arg" id="appkey" maxlength="32" size="34" value="{}">\n',\
                '<input type="text" class="arg" id="iv_index" maxlength="8" size="8" value="{}">\n',\
                '<input type="text" class="arg" id="seq" maxlength="6" size="6" value="{}">\n',\
                '<div class="fixed">Calculated: {} initially, incremented for each PDU</div>\n',\
                '<option class="field" value="{}" selected>{}</option>\n',\
                '<option class="field" value="{}" >{}</option>\n',\
                '<option class="field" value="{}" >{}</option>\n',\
                '<option class="field" value="{}" >{}</option>\n'
                ]
    lines_to_write = []
    for line in index_file:    
        for key_item in key_words:
            if key_item in line:
                _index = key_words.index(key_item)
                try:
                    print("Replacing for index "+str(_index))
                    if type(params[_index]) is list:
                        line = template[_index].format(params[_index][0],params[_index][0]+" "+params[_index][1])
                    else:
                        line = template[_index].format(params[_index])
                except:
                    print("Skipped replacing for index  "+str(_index))
        lines_to_write.append(line)
    new_file = open("mod.html","w")
    new_file.writelines(lines_to_write)
    new_file.close()
    print("JSON to HTML Patch complete!")

if __name__ == "__main__":
    print("Patching files with new configuration")
    choice = input("Do you wan to continue? y/n \n")
    if choice[0].lower() == "y":
        print("Patching Now")
        files = [f for f in os.listdir('.') if os.path.isfile(f)]
        json_file = ""
        index_file = ""
        for f in files:
            if ".json" in f:
                json_file = f
                break
        
        for f in files:
            if "index" in f:
                index_file = f
        

        
        if json_file == "":
            print("Configuration file not found in current working directory!")
            quit()
            
        else:
            print("Using config data from " + json_file)
            json_file_1 = open(json_file,"r")
            json_file_2 = open(json_file,"r")
            
            
        if index_file == "":
            print("Index file not found in current working directory!")
            quit()
            
        else:
            print("Modifying homepage file " + index_file)
            index_file = open(index_file,"r")
            patch_index(json_file_1, index_file)
        
        pwd = os.getcwd()
        files = [f for f in os.listdir(pwd+'\\js') if os.path.isfile(pwd+'\\js\\'+f) ]
        app_js_file = ""
        for f in files:
            if "app_template.js" in f:
                app_js_file = pwd+"\\js\\app_template.js"
                break
        
        if app_js_file == "":
            print("app_template.js not foun in \\js directory!")
            quit()
        
        else:
            print("Modifying app.js file ")
            app_js_file = open(app_js_file,"r")
            patch_app_js(json_file_2, app_js_file, pwd)
        
    else:
        print("No changes made, quitting!")
        
    in_dummy = input("Press any key to continue")    
        