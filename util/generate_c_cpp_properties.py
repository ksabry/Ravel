import os
import json
import re

UTIL_PATH = os.path.dirname(os.path.realpath(__file__))
PROJECT_PATH = os.path.join(UTIL_PATH, "..")
SRC_PATH = os.path.join(PROJECT_PATH, "src")

def getPropertiesBase():
    global PROJECT_PATH
    with open(os.path.join(PROJECT_PATH, ".vscode/c_cpp_properties_base.json")) as file:
        return json.load(file)

def applyReplacements(string, replacements):
    if not isinstance(string, str):
        return string
    for rep_key, rep_value in replacements.items():
        if isinstance(rep_value, str):
            string = string.replace(rep_key, rep_value)
        elif string == rep_key:
            return rep_value
    return string

def populate(container, replacements):
    if isinstance(container, dict):
        populated = {}
        for key, value in container.items():
            populated[applyReplacements(key, replacements)] = populate(value, replacements)
        return populated

    elif isinstance(container, list):
        populated = []
        for element in container:
            if isinstance(element, str):
                populated.append(applyReplacements(element, replacements))
            else:
                populated.append(populate(element, replacements))
        return populated

    elif isinstance(container, str):
        return applyReplacements(container, replacements)

    else:
        return container

def expandPath(path):
    path = path.replace("${workspaceRoot}", PROJECT_PATH)
    result = [root for root, _, _ in os.walk(path)]
    return [path.replace(PROJECT_PATH, "${workspaceRoot}") for path in result]

def expandIncludePath(properties):
    if "configurations" in properties:
        for config in properties["configurations"]:
            if "includePathRecursive" in config:
                includePath = [path for rpath in config["includePathRecursive"] for path in expandPath(rpath)]
                if "includePath" in config:
                    includePath += config["includePath"]
                
                if "includePathExcludeRegex" in config:
                    regexes = config["includePathExcludeRegex"]
                    if isinstance(regexes, str):
                        regexes = [regexes]
                    for regex in regexes:
                        includePath = [path for path in includePath if not re.match("^" + regex + "$", path)]
                    del config["includePathExcludeRegex"]

                config["includePath"] = includePath
                del config["includePathRecursive"]

def generateProperties(properties_base):
    replacements = {
    }
    propertes = populate(properties_base, replacements)
    expandIncludePath(propertes)
    propertes["__WARNING"] = "This file is generated and changes WILL NOT be kept; Refer to c_cpp_properties_base.json to make permanent changes or utils/generate_c_cpp_properties.py for generation script"
    return propertes

def writeJson(obj, filename):
    with open(filename, 'w') as file:
        json.dump(obj, file, sort_keys=True, indent=4)

properties = generateProperties(getPropertiesBase())
writeJson(properties, os.path.join(PROJECT_PATH, ".vscode/c_cpp_properties.json"))
print("Json generated successfully")