for file in *.tbl; do
    if [[ ! "$file" =~ - ]]; then
        mv "$file" "${file%.tbl}-1.tbl"
    fi
done
