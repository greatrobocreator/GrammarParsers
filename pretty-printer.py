def vector_to_list(std_vector):
    out_list = []
    value_reference = std_vector['_M_impl']['_M_start']
    while value_reference != std_vector['_M_impl']['_M_finish']:
        out_list.append(chr(int(value_reference.dereference())))
        value_reference += 1

    return out_list

class ConfigurationPrinter:
    def __init__(self, val):
        # val is the python representation of you C++ variable.
        # It is a "gdb.Value" object and you can query the member
        # atributes of the C++ object as below. Since the result is
        # another "gdb.Value" I'am converting it to a python float
        self.val = val

    # Whatever the `to_string` method returns is what will be printed in
    # gdb when this pretty-printer is used
    def to_string(self):
        s = ''.join(vector_to_list(self.val['rule']['to']))
        k = self.val['k']
        #return k
        return "( {} -> {}.{}, {} )".format(chr(int(self.val['rule']['from'])), s[:k], s[k:], self.val['i'])
        #return ''.join(vector_to_list(self.val['rule']['to']))
        #return str(self.val['rule']['to']['_M_impl']['_M_start'])



import gdb.printing
# Create a "collection" of pretty-printers
# Note that the argument passed to "RegexpCollectionPrettyPrinter" is the name of the pretty-printer and you can choose your own
pp = gdb.printing.RegexpCollectionPrettyPrinter('cppsim')
# Register a pretty-printer for the Coordinate class. The second argument is a
# regular expression and my Coordinate class is in a namespace called `cppsim`
pp.add_printer('Configuration', '^Configuration$', ConfigurationPrinter)
# Register our collection into GDB
gdb.printing.register_pretty_printer(gdb.current_objfile(), pp, replace=True)