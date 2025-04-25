# SavJsonConverter Plugin Documentation

## 🔧 How to Use

You can find the **SavJsonConverter** menu in the editor toolbar:

![Menu Location](https://github.com/user-attachments/assets/23cc44fa-d5ea-40d8-b4d5-ed8a1294adee)  
![Plugin Window](https://github.com/user-attachments/assets/599c6ac0-d2f9-40b6-b64f-81537f12bdb2)

This plugin provides the following core features:

- Load `.sav` files and convert them to `.json`
- Load `.json` files and convert them to `.sav`  
  ⚠️ *The save class you're trying to convert must be declared within your module*
- Serialize and test custom save data

![Conversion Preview](https://github.com/user-attachments/assets/15e265e9-8b1a-430d-8987-a98b216137b4)

If the plugin detects invalid or unexpected data during `.sav` file parsing, it will display detailed warnings in the log.

---

## 🧩 Custom Serialization Support

### For `UObject`-derived classes

To serialize your own `UObject`-based save classes, override the `Serialize` function and manually serialize each member variable using the archive:


![UObject Serialize Example](https://github.com/user-attachments/assets/82c86ec3-8985-4893-8f9f-a7eddd261ab3)

---

### For `struct` types

For structs, overload the `operator<<` and serialize only the desired fields:


![Struct Serialization Example](https://github.com/user-attachments/assets/ebd1f170-3678-4e6c-809f-1ce1dfd0ad2b)

---

## ⚠️ Important Notes

- If you've modified the `.sav` file externally (e.g., via another tool), make sure to **reload** it before attempting conversion.
- The `SaveGameClass` specified in the `.json` must be **included and declared within your project module**.
- If you want to see a test example, refer to the TestSaveData file in the Test folder.
