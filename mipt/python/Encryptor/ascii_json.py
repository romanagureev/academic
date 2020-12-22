import json


def load_ascii(file_handle):
    return _ascii(
        json.load(file_handle, object_hook=_ascii),
        ignore_dicts=True
    )


def loads_ascii(json_text):
    return _ascii(
        json.loads(json_text, object_hook=_ascii),
        ignore_dicts=True
    )


def _ascii(data, ignore_dicts=False):
    # if this is a unicode string, return its string representation
    try:
        return data.encode('utf-8')
    except:
        if isinstance(data, str):
            return data
    # if this is a list of values, return list of ascii values
    if isinstance(data, list):
        return [_ascii(item, ignore_dicts=True) for item in data]
    # if this is a dictionary, return dictionary of ascii keys and values
    # but only if we haven't already ascii it
    if isinstance(data, dict) and not ignore_dicts:
        return {
            _ascii(key, ignore_dicts=True): _ascii(value, ignore_dicts=True)
            for key, value in data.items()
        }
    # if it anything else, return it in its original form
    return data
