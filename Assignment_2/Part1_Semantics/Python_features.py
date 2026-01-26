def analyze_python_features():
    print("Python Feature Analysis")

    # 1. Feature: Dynamic Typing
    # We demonstrate that a variable can hold different types at different times.
    val = 100
    print(f"Value: {val}, Type: {type(val)}")
    
    val = "Now I am a String"
    print(f"Value: {val}, Type: {type(val)}")

    # 2. Feature: Scope 'Leaking'
    # In many languages, variables inside a loop die when the loop ends.
    # In Python, they 'leak' out and stay alive.
    print("[Scope] Starting loop...")
    for i in range(3):
        loop_variable = "I am still available outside the loop!"
    
    # This proves the variable leaked out of the loop scope
    print(f"[Scope] Loop finished, but variable exists: '{loop_variable}'")

analyze_python_features()