import csv
import json
import argparse
from unidecode import unidecode

# THIS IS FOR GENERATING A NEW PINEAPPLE RECON OUI LIST
#
def strip_non_alphanumeric(text):
    allowed_chars = set("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789.,& ")
    cleaned_text = ''.join(char if char in allowed_chars else ' ' for char in unidecode(text))
    cleaned_text = ' '.join(cleaned_text.split())  # Replace multiple spaces with a single space
    return cleaned_text

def process_csv(input_file, output_file):
    with open(input_file, newline='', encoding='utf-8') as csvfile:
        reader = csv.reader(csvfile)
        header = next(reader)
        mac_prefix_index = header.index('Mac Prefix')
        vendor_name_index = header.index('Vendor Name')
        block_type_index = header.index('Block Type')

        mac_dict = {}

        for row in reader:
            mac_prefix_value = row[mac_prefix_index].replace(":", "").replace("-", "")
            vendor_name = strip_non_alphanumeric(row[vendor_name_index])
            block_type = row[block_type_index]

            if block_type == 'MA-L':
                mac_dict[mac_prefix_value] = vendor_name

    # Sort results by the length of the Mac Prefix (smallest to largest),
    # and then by Mac Prefix in ascending order
    sorted_mac_dict = {k: v for k, v in sorted(mac_dict.items(), key=lambda x: (len(x[0]), x[0]))}

    with open(output_file, 'w', encoding='utf-8') as jsonfile:
        json.dump(sorted_mac_dict, jsonfile, ensure_ascii=False)

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='Process CSV files for specified Block Types.')
    parser.add_argument('-i', '--input', type=str, help='Input CSV file paths', nargs='+', required=True)
    parser.add_argument('-o', '--output', type=str, help='Output JSON file path', required=True)
    args = parser.parse_args()

    for input_file in args.input:
        process_csv(input_file, args.output)