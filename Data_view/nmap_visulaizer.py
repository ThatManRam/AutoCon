import sys
import xml.etree.ElementTree as ET
from pyvis.network import Network

if len(sys.argv) != 2:
    print(f"Usage: python3 {sys.argv[0]} scan.xml")
    sys.exit(1)

xml_file = sys.argv[1]

try:
    tree = ET.parse(xml_file)
    root = tree.getroot()
except Exception as e:
    print(f"Failed to read XML: {e}")
    sys.exit(1)

# Create interactive browser map
net = Network(
    height="850px",
    width="100%",
    bgcolor="#111827",
    font_color="white",
    directed=False
)

# Enable draggable physics layout
net.force_atlas_2based()
net.show_buttons(filter_=["physics"])

# Central scan node
net.add_node(
    "scan_root",
    label="Nmap Scan",
    title="Root node for discovered hosts",
    shape="dot",
    size=30
)

for host in root.findall("host"):
    status = host.find("status")
    if status is None or status.get("state") != "up":
        continue

    # Get IP address
    ip = None
    for address in host.findall("address"):
        if address.get("addrtype") in ("ipv4", "ipv6"):
            ip = address.get("addr")
            break

    if not ip:
        continue

    # Get hostname if present
    hostname = ""
    hostnames = host.find("hostnames")
    if hostnames is not None:
        hostname_entry = hostnames.find("hostname")
        if hostname_entry is not None:
            hostname = hostname_entry.get("name", "")

    # Get open ports
    open_ports = []
    ports = host.find("ports")
    if ports is not None:
        for port in ports.findall("port"):
            state = port.find("state")
            if state is None or state.get("state") != "open":
                continue

            portid = port.get("portid", "?")
            protocol = port.get("protocol", "?")

            service = port.find("service")
            service_name = service.get("name", "unknown") if service is not None else "unknown"

            open_ports.append(f"{portid}/{protocol} - {service_name}")

    # Text shown on the node
    label = ip
    if hostname:
        label += f"\n{hostname}"

    # Hover tooltip
    tooltip = f"<b>{ip}</b>"
    if hostname:
        tooltip += f"<br>Hostname: {hostname}"

    if open_ports:
        tooltip += "<br><br><b>Open ports:</b><br>" + "<br>".join(open_ports)
    else:
        tooltip += "<br><br>No open ports listed"

    # Add host node
    net.add_node(
        ip,
        label=label,
        title=tooltip,
        shape="box",
        size=20
    )

    # Connect host to scan root
    net.add_edge("scan_root", ip)

# Save HTML output
output_file = "nmap_interactive_map.html"
net.write_html(output_file)

print(f"Created interactive map: {output_file}")