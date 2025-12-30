#!/usr/bin/env python3
import json

with open('/development/workspace/VanSight/VanSightDisplayClient/squareline/VanSightDisplayClient.spj', 'r') as f:
    data = json.load(f)

def find_all_names(obj, path="root"):
    """Tüm Name property'lerini bul"""
    results = []
    
    if isinstance(obj, dict):
        if "properties" in obj:
            for prop in obj["properties"]:
                if prop.get("strtype") == "OBJECT/Name":
                    results.append({
                        "name": prop.get("strval"),
                        "path": path,
                        "has_compnid": "compnid" in prop
                    })
        
        for key, value in obj.items():
            results.extend(find_all_names(value, f"{path}.{key}"))
    
    elif isinstance(obj, list):
        for i, item in enumerate(obj):
            results.extend(find_all_names(item, f"{path}[{i}]"))
    
    return results

# Tüm isimleri bul
all_names = find_all_names(data)

# btn ile başlayanları filtrele
button_names = [r for r in all_names if r["name"] and r["name"].startswith("btn")]

print(f"Found {len(button_names)} buttons:")
for btn in button_names:
    print(f"  • {btn['name']} (compnid: {btn['has_compnid']})")
