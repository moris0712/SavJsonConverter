# SavJsonConverter Plugin Documentation

## How to Use

You can find the **SavJsonConverter** menu in the editor tab:

![Image](https://github.com/user-attachments/assets/23cc44fa-d5ea-40d8-b4d5-ed8a1294adee)

![Image](https://github.com/user-attachments/assets/509b25fe-2a0f-40e4-9b70-45fd25f12591)

This plugin provides the following features:
- Load `.sav` files and convert/save them to `.json`
- Load `.json` files and convert/save them to `.sav`
- Serialize test data


![Image](https://github.com/user-attachments/assets/f63527d8-8935-4ef1-9e03-37e202a30e41)

If any unexpected or invalid data is detected in the .sav file, you can check the log for detailed warnings.

---

### 🧩 Custom Serialization

#### For `UObject`-derived instances

If you want to serialize your custom `UObject`-derived classes, override the `Serialize` function  
and manually bind each member variable to the archive like this:

![Image](https://github.com/user-attachments/assets/82c86ec3-8985-4893-8f9f-a7eddd261ab3)

---

#### For `struct` types

If you're working with `struct` instances, you need to overload the `operator<<`  
and serialize the desired member variables the same way:

![Image](https://github.com/user-attachments/assets/ebd1f170-3678-4e6c-809f-1ce1dfd0ad2b)
---

### ⚠️ Notes

- If the loaded file has been modified externally, **reload it before converting.**
- The `SaveGameClass` you're trying to convert must be **included in your project.**
