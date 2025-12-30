#!/usr/bin/env python3
"""
SquareLine Studio Event Handler Adder
Bu script .spj dosyasına tüm butonlar için CLICKED event'leri ekler
"""

import json
import sys

# Buton adları ve event handler fonksiyonları
BUTTON_EVENTS = {
    "btnHome": "onBtnHomeClick",
    "btnLeftChair": "onBtnLeftChairClick",
    "btnRightChair": "onBtnRightChairClick",
    "btnKitchen": "onBtnKitchenClick",
    "btnToilet": "onBtnToiletClick",
    "btnBath": "onBtnBathClick",
    "btnBed": "onBtnBedClick",
    "btnBalkonyUp": "onBtnBalkonyUpClick",
    "btnBalkonyDown": "onBtnBalkonyDownClick",
    "btnProjector": "onBtnProjectorClick",
    "btnReloadInformation": "onBtnReloadInformationClick",
    "btnCloseAll": "onBtnCloseAllClick"
}

def create_event_handler(function_name):
    """CLICKED event handler yapısı oluşturur"""
    return {
        "disabled": False,
        "nid": 1000206,
        "strtype": "_event/EventHandler",
        "strval": "CLICKED",
        "childs": [
            {
                "nid": 1000207,
                "strtype": "_event/name",
                "strval": function_name,
                "InheritedType": 10
            },
            {
                "nid": 1000208,
                "strtype": "_event/condition_C",
                "strval": "",
                "InheritedType": 10
            },
            {
                "nid": 1000209,
                "strtype": "_event/condition_P",
                "strval": "",
                "InheritedType": 10
            },
            {
                "nid": 1000216,
                "strtype": "_event/action",
                "strval": "CALL FUNCTION",
                "childs": [
                    {
                        "nid": 1000217,
                        "strtype": "CALL FUNCTION/Name",
                        "strval": "CALL FUNCTION",
                        "InheritedType": 10
                    },
                    {
                        "nid": 1000218,
                        "strtype": "CALL FUNCTION/Call",
                        "strval": "<{Function_name}>( event_struct )",
                        "InheritedType": 10
                    },
                    {
                        "nid": 1000219,
                        "strtype": "CALL FUNCTION/CallC",
                        "strval": "<{Function_name}>( e );",
                        "InheritedType": 10
                    },
                    {
                        "nid": 1000220,
                        "strtype": "CALL FUNCTION/Function_name",
                        "strval": function_name,
                        "InheritedType": 10
                    },
                    {
                        "nid": 1000221,
                        "strtype": "CALL FUNCTION/Dont_export_function",
                        "strval": "True",
                        "InheritedType": 2
                    }
                ],
                "InheritedType": 10
            }
        ],
        "InheritedType": 4
    }

def find_button_by_name(obj, button_name):
    """Recursive olarak button'u bulur (component instance olarak)"""
    if isinstance(obj, dict):
        # Properties içinde Name kontrolü (compnid olabilir)
        if "properties" in obj:
            for prop in obj["properties"]:
                # Component instance için compnid kontrolü ekle
                if (prop.get("strtype") == "OBJECT/Name" and 
                    prop.get("strval") == button_name):
                    return obj
        
        # Children içinde ara
        if "children" in obj:
            for child in obj["children"]:
                result = find_button_by_name(child, button_name)
                if result:
                    return result
    
    elif isinstance(obj, list):
        for item in obj:
            result = find_button_by_name(item, button_name)
            if result:
                return result
    
    return None

def has_event_handler(button_obj):
    """Button'da zaten event handler var mı kontrol eder"""
    if "properties" in button_obj:
        for prop in button_obj["properties"]:
            if prop.get("strtype") == "_event/EventHandler":
                return True
    return False

def add_event_to_button(button_obj, function_name):
    """Button'a event handler ekler"""
    if "properties" not in button_obj:
        return False
    
    # Event handler'ı properties'in sonuna ekle
    event_handler = create_event_handler(function_name)
    button_obj["properties"].append(event_handler)
    return True

def main(spj_file_path):
    print(f"📖 Reading {spj_file_path}...")
    
    # JSON dosyasını oku
    with open(spj_file_path, 'r', encoding='utf-8') as f:
        data = json.load(f)
    
    print(f"✓ File loaded successfully")
    print(f"\n🔍 Processing buttons...")
    
    added_count = 0
    skipped_count = 0
    
    # Her buton için event ekle
    for button_name, function_name in BUTTON_EVENTS.items():
        print(f"\n  • {button_name} -> {function_name}")
        
        # Button'u bul (root'tan başla)
        button_obj = find_button_by_name(data.get("root", data), button_name)
        
        if not button_obj:
            print(f"    ⚠️  Button not found!")
            continue
        
        # Zaten event var mı kontrol et
        if has_event_handler(button_obj):
            print(f"    ⏭️  Event already exists, skipping")
            skipped_count += 1
            continue
        
        # Event ekle
        if add_event_to_button(button_obj, function_name):
            print(f"    ✅ Event added")
            added_count += 1
        else:
            print(f"    ❌ Failed to add event")
    
    print(f"\n📊 Summary:")
    print(f"  • Events added: {added_count}")
    print(f"  • Events skipped (already exist): {skipped_count}")
    print(f"  • Total buttons: {len(BUTTON_EVENTS)}")
    
    # Backup oluştur
    backup_path = spj_file_path + ".backup"
    print(f"\n💾 Creating backup: {backup_path}")
    with open(backup_path, 'w', encoding='utf-8') as f:
        json.dump(data, f, indent=2, ensure_ascii=False)
    
    # Değişiklikleri kaydet
    print(f"💾 Saving changes to: {spj_file_path}")
    with open(spj_file_path, 'w', encoding='utf-8') as f:
        json.dump(data, f, indent=2, ensure_ascii=False)
    
    print(f"\n✅ Done! Please open SquareLine Studio and export the UI.")
    print(f"\nℹ️  Backup saved as: {backup_path}")

if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("Usage: python3 add_events.py <path_to_spj_file>")
        sys.exit(1)
    
    main(sys.argv[1])
