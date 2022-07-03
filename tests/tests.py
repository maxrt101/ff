
tests = {
# lang
    'lang/assert_false': {
        'expect': 'return',
        'value': 1
    },
    'lang/assert_true': {
        'expect': 'return',
        'value': 0
    },
    'lang/cast': {
        'expect': 'return',
        'value': 0
    },
    'lang/const_global': {
        'expect': 'return',
        'value': 0
    },
    'lang/const_local': {
        'expect': 'return',
        'value': 0
    },
    'lang/const_set_global': {
        'expect': 'return',
        'value': 1
    },
    'lang/const_set_local': {
        'expect': 'return',
        'value': 1
    },
    'lang/fields': {
        'expect': 'return',
        'value': 0
    },
    'lang/fn_bad_return': {
        'expect': 'return',
        'value': 1
    },
    'lang/fn_different_returns': {
        'expect': 'return',
        'value': 1
    },
    'lang/fn_empty': {
        'expect': 'return',
        'value': 0
    },
    'lang/fn_entry': {
        'expect': 'return',
        'args': '-s entry=test',
        'value': 0
    },
    'lang/fn_oneline': {
        'expect': 'return',
        'value': 0
    },
    'lang/fn_ref': {
        'expect': 'return',
        'value': 0
    },
    'lang/fn_return': {
        'expect': 'return',
        'value': 0
    },
    'lang/if': {
        'expect': 'stdout',
        'value': 'a=0\na!=1\n'
    },
    'lang/import': {
        'expect': 'return',
        'value': 0
    },
    'lang/loop_for': {
        'expect': 'return',
        'value': 0
    },
    'lang/loop_loop': {
        'expect': 'return',
        'value': 0
    },
    'lang/loop_while': {
        'expect': 'return',
        'value': 0
    },
    'lang/module_test': {
        'expect': 'return',
        'value': 0
    },
    'lang/recursion': {
        'expect': 'return',
        'value': 0
    },
    'lang/var_global': {
        'expect': 'return',
        'value': 0
    },
    'lang/var_local': {
        'expect': 'return',
        'value': 0
    },
    'lang/var_reassign_different_type': {
        'expect': 'return',
        'value': 1
    },
    'lang/var_ref': {
        'expect': 'return',
        'value': 0
    },

# types
    'types/bool': {
        'expect': 'return',
        'value': 0
    },
    'types/float': {
        'expect': 'return',
        'value': 0
    },
    'types/int': {
        'expect': 'return',
        'value': 0
    },
    'types/string': {
        'expect': 'return',
        'value': 0
    },
    'types/vector': {
        'expect': 'return',
        'value': 0
    },
    'types/dict': {
        'expect': 'return',
        'value': 0
    }

# stdlib
}
