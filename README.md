# 📦 SavJsonConverter Plugin Documentation

SavJsonConverter - [FabUrl](https://www.fab.com/listings/8c420402-3221-4d21-b921-8622af63b4d3)

Document - [한국어](https://github.com/moris0712/SavJsonConverter/wiki/Document)

## 🛠️ How to Use

You can access **SavJsonConverter** from the editor toolbar:

📍 **Menu Location**  
![Menu Location](https://github.com/user-attachments/assets/23cc44fa-d5ea-40d8-b4d5-ed8a1294adee)  
🫟 **Plugin Window**  
![Plugin Window](https://github.com/user-attachments/assets/599c6ac0-d2f9-40b6-b64f-81537f12bdb2)

### 🔁 Core Features

- Convert `.sav` files to `.json`
- Convert `.json` files to `.sav`  
  ⚠️ *The save class must be declared within your module.*
- Serialize and test custom save data structures

🔍 **Conversion Preview**  
![Conversion Preview](https://github.com/user-attachments/assets/15e265e9-8b1a-430d-8987-a98b216137b4)

📢 If invalid or unexpected data is detected while parsing `.sav` files, detailed warnings will be shown in the output log.

---

## 🧹 Custom Serialization Support

### 📌 For `UObject`-Derived Classes

To serialize custom `UObject`-based save classes, override the `Serialize()` function and manually handle each property using the archive:

📄 **UObject Serialization Example**  
![UObject Serialize Example](https://github.com/user-attachments/assets/82c86ec3-8985-4893-8f9f-a7eddd261ab3)

---

### 📌 For `struct` Types

To serialize custom `structs`, overload the `operator<<` and serialize only the fields you need:

📄 **Struct Serialization Example**  
![Struct Serialization Example](https://github.com/user-attachments/assets/ebd1f170-3678-4e6c-809f-1ce1dfd0ad2b)

---

## 💾 Load / Save SaveGame

> ⚡️ **Custom serialization support must be implemented beforehand.**

### 📅 Saving `UObject` Data in a SaveGame

Use the provided utility to serialize a `UObject` into raw bytes before saving:

🧱 **UObject ➔ Bytes**  
![UObjectToBytes](https://github.com/user-attachments/assets/dfa70f92-4082-4bc9-9079-bbbde003c3d2)

---

### 📄 Loading `UObject` from a Saved SaveGame

Deserialize the byte array back into a valid `UObject` instance:

🧱 **Bytes ➔ UObject**  
![BytesToUObject](https://github.com/user-attachments/assets/c1fb340a-e457-4105-a95b-9129a8e7aab0)

---

## ⚠️ Important Notes

- 🌀 If you modify `.sav` files externally (e.g., with another tool), **reload** them before converting.
- 📦 The `SaveGameClass` in the `.json` must be declared and included within your project module.
- 🔹 **✨ Want to see a test example? Check out the `TestSaveData` file inside the `Test` folder.**
  
  > 🔥 This is the best reference for understanding real usage. Don’t miss it!

