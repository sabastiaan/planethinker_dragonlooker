import json
import random
import string
import sys

def random_string(length=10):
    return ''.join(random.choices(string.ascii_letters + string.digits, k=length))

def random_json(depth=3, breadth=3):
    if depth == 0:
        # Base case: return a random value
        value_type = random.choice(['string', 'int', 'array'])
        if value_type == 'string':
            return random_string()
        elif value_type == 'int':
            return random.randint(0, 1000)
        elif value_type == 'array':
            return [random.randint(0, 100) for _ in range(random.randint(1, 5))]
    else:
        obj = {}
        for _ in range(breadth):
            key = random_string()
            obj[key] = random_json(depth - 1, breadth)
        # Occasionally add an array as a value
        if random.random() > 0.5:
            obj[random_string()] = [random_json(depth - 1, breadth) for _ in range(random.randint(1, 3))]
        return obj

def generate_json_file(filepath, size_in_bytes):
    # Create a random JSON object and serialize it until we reach the required size
    data = random_json(depth=3, breadth=3)
    json_str = json.dumps(data, indent=2)

    while len(json_str.encode('utf-8')) < size_in_bytes:
        # Increase size by adding more nested objects or expanding arrays
        data[random_string()] = random_json(depth=3, breadth=3)
        json_str = json.dumps(data, indent=2)

    with open(filepath, 'w') as f:
        f.write(json_str)
    print(f"Generated JSON file at: {filepath}")
    
    # Return a random deep path within the JSON structure, including array indices
    path = []
    current = data
    while isinstance(current, dict) or isinstance(current, list):
        if isinstance(current, dict):
            key = random.choice(list(current.keys()))
            path.append(key)
            current = current[key]
        elif isinstance(current, list) and current:
            index = random.randint(0, len(current) - 1)
            path.append(f"[{index}]")
            current = current[index]
    path_str = '.'.join(path)
    print(f"Path to a deep value: {path_str}")
    return path_str

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: generate_json.py <file_path> <size_in_bytes>")
        sys.exit(1)
    filepath = sys.argv[1]
    size_in_bytes = int(sys.argv[2])
    path = generate_json_file(filepath, size_in_bytes)
    with open(filepath + ".path", 'w') as path_file:
        path_file.write(path)
