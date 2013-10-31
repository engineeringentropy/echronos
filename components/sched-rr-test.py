from prj import Module


class SchedRrTestModule(Module):
    xml_schema = """
  <schema>
   <entry name="prefix" type="ident" optional="true" />
   <entry name="semaphores" type="list" default="[]" auto_index_field="idx">
     <entry name="semaphore" type="dict">
      <entry name="name" type="ident" />
     </entry>
   </entry>
   <entry name="tasks" type="list" auto_index_field="idx">
     <entry name="task" type="dict">
      <entry name="name" type="ident" />
     </entry>
   </entry>
  </schema>
"""
    files = [
        {'input': 'rtos-sched-rr-test.h', 'render': True},
        {'input': 'rtos-sched-rr-test.c', 'render': True, 'type': 'c'},
    ]

    def configure(self, xml_config):
        config = super().configure(xml_config)

        config['prefix_func'] = config['prefix'] + '_' if config['prefix'] is not None else ''
        config['prefix_type'] = config['prefix'].capitalize() if config['prefix'] is not None else ''
        config['prefix_const'] = config['prefix'].upper() + '_' if config['prefix'] is not None else ''

        return config

module = SchedRrTestModule()
