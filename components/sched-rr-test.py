from prj import Module


class SchedRrTestModule(Module):
    xml_schema = """
  <schema>
   <entry name="prefix" type="c_ident" default="rtos_" />
   <entry name="semaphores" type="list" auto_index_field="idx">
     <entry name="semaphore" type="dict">
      <entry name="name" type="c_ident" />
     </entry>
   </entry>
   <entry name="tasks" type="list" auto_index_field="idx">
     <entry name="task" type="dict">
      <entry name="name" type="c_ident" />
     </entry>
   </entry>
  </schema>
"""
    files = [
        {'input': 'rtos-sched-rr-test.h', 'render': True},
        {'input': 'rtos-sched-rr-test.c', 'render': True, 'type': 'c'},
    ]

module = SchedRrTestModule()
