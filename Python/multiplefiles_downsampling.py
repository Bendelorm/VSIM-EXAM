import laspy

# List of input files
input_files = [
    'las2/test1.laz',
    'las2/test2.laz',
    'las2/test3.laz',
    'las2/test4.laz',
    'las2/test5.laz',
    'las2/test6.laz'
]
output_file = 'las2/lasdata2.txt'

# Downsample factor (10 = keep 10%, 20 = keep 5%, etc.)
downsample_factor = 10

# Write all points to a single file
with open(output_file, 'w') as f:
    for input_file in input_files:
        print(f"Processing {input_file}...")
        las = laspy.read(input_file, laz_backend=laspy.LazBackend.Laszip)
        
        original_count = len(las.x)
        kept_count = 0
        
        # Keep every Nth point
        for i, (x, y, z) in enumerate(zip(las.x, las.y, las.z)):
            if i % downsample_factor == 0:
                f.write(f"{x} {y} {z}\n")
                kept_count += 1
        
        print(f"  Original: {original_count} points, Kept: {kept_count} points ({kept_count/original_count*100:.1f}%)")

print(f"All LAZ files exported to '{output_file}' with downsampling")