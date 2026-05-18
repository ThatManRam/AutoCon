import sys
import json
import xml.etree.ElementTree as ET
from pathlib import Path


def get_text_attr(element, attr, default="Unknown"):
    return element.get(attr, default) if element is not None else default


def parse_nmap_xml(xml_file):
    tree = ET.parse(xml_file)
    root = tree.getroot()

    hosts = []

    for host in root.findall("host"):
        status = host.find("status")
        if status is not None and status.get("state") != "up":
            continue

        ip = None
        mac = "Unknown"
        vendor = "Unknown"

        for address in host.findall("address"):
            addr_type = address.get("addrtype")

            if addr_type in ("ipv4", "ipv6"):
                ip = address.get("addr")
            elif addr_type == "mac":
                mac = address.get("addr", "Unknown")
                vendor = address.get("vendor", "Unknown")

        if not ip:
            continue

        hostname = "Unknown"
        hostname_element = host.find("./hostnames/hostname")
        if hostname_element is not None:
            hostname = hostname_element.get("name", "Unknown")

        ports = []
        for port in host.findall("./ports/port"):
            state = port.find("state")
            if state is None or state.get("state") != "open":
                continue

            service = port.find("service")

            port_info = {
                "port": port.get("portid", "Unknown"),
                "protocol": port.get("protocol", "Unknown"),
                "service": get_text_attr(service, "name"),
                "product": get_text_attr(service, "product", ""),
                "version": get_text_attr(service, "version", ""),
                "extra": get_text_attr(service, "extrainfo", "")
            }

            ports.append(port_info)

        os_guesses = []
        for osmatch in host.findall("./os/osmatch"):
            os_guesses.append({
                "name": osmatch.get("name", "Unknown"),
                "accuracy": osmatch.get("accuracy", "Unknown")
            })

        hosts.append({
            "ip": ip,
            "hostname": hostname,
            "mac": mac,
            "vendor": vendor,
            "ports": ports,
            "os": os_guesses
        })

    return hosts


def build_html(hosts, output_file="nmap_interactive_map.html"):
    nodes = [
        {
            "id": "network",
            "label": "Scanned Network",
            "shape": "dot",
            "size": 30
        }
    ]

    edges = []

    host_lookup = {}

    for host in hosts:
        ip = host["ip"]

        nodes.append({
            "id": ip,
            "label": ip,
            "shape": "box",
            "size": 22
        })

        edges.append({
            "from": "network",
            "to": ip
        })

        host_lookup[ip] = host

    html = f"""<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
    <title>Nmap Interactive Network Map</title>

    <script type="text/javascript"
        src="https://unpkg.com/vis-network/standalone/umd/vis-network.min.js">
    </script>

    <style>
        body {{
            margin: 0;
            font-family: Arial, sans-serif;
            background: #111827;
            color: #f9fafb;
        }}

        .container {{
            display: flex;
            height: 100vh;
        }}

        #network {{
            width: 72%;
            height: 100vh;
            border-right: 1px solid #374151;
        }}

        #details {{
            width: 28%;
            padding: 24px;
            overflow-y: auto;
            background: #1f2937;
        }}

        h1 {{
            font-size: 24px;
            margin-top: 0;
        }}

        h2 {{
            font-size: 20px;
            margin-bottom: 8px;
        }}

        .muted {{
            color: #9ca3af;
        }}

        .section {{
            margin-top: 22px;
        }}

        .port-card {{
            background: #111827;
            border: 1px solid #374151;
            border-radius: 10px;
            padding: 12px;
            margin-top: 10px;
        }}

        .os-card {{
            background: #111827;
            border: 1px solid #374151;
            border-radius: 10px;
            padding: 12px;
            margin-top: 10px;
        }}

        .label {{
            color: #93c5fd;
            font-weight: bold;
        }}
    </style>
</head>

<body>
    <div class="container">
        <div id="network"></div>

        <div id="details">
            <h1>Host Details</h1>
            <p class="muted">Click an IP address on the map to view its scan details.</p>
        </div>
    </div>

    <script>
        const nodes = new vis.DataSet({json.dumps(nodes)});
        const edges = new vis.DataSet({json.dumps(edges)});
        const hostData = {json.dumps(host_lookup)};

        const container = document.getElementById("network");

        const data = {{
            nodes: nodes,
            edges: edges
        }};

        const options = {{
            physics: {{
                enabled: true,
                barnesHut: {{
                    gravitationalConstant: -8000,
                    springLength: 180,
                    springConstant: 0.04
                }}
            }},
            interaction: {{
                hover: true,
                navigationButtons: true,
                keyboard: true
            }},
            nodes: {{
                font: {{
                    color: "#f9fafb",
                    size: 16
                }},
                color: {{
                    background: "#2563eb",
                    border: "#93c5fd",
                    highlight: {{
                        background: "#60a5fa",
                        border: "#dbeafe"
                    }}
                }},
                margin: 12
            }},
            edges: {{
                color: "#6b7280",
                smooth: true
            }}
        }};

        const network = new vis.Network(container, data, options);

        network.on("click", function(params) {{
            const details = document.getElementById("details");

            if (params.nodes.length === 0) {{
                details.innerHTML = `
                    <h1>Host Details</h1>
                    <p class="muted">Click an IP address on the map to view its scan details.</p>
                `;
                return;
            }}

            const nodeId = params.nodes[0];

            if (nodeId === "network") {{
                details.innerHTML = `
                    <h1>Scanned Network</h1>
                    <p class="muted">Select an IP node to view host details.</p>
                `;
                return;
            }}

            const host = hostData[nodeId];

            let portsHtml = "";
            if (host.ports.length > 0) {{
                portsHtml = host.ports.map(port => `
                    <div class="port-card">
                        <div><span class="label">Port:</span> ${{port.port}}/${{port.protocol}}</div>
                        <div><span class="label">Service:</span> ${{port.service}}</div>
                        ${{port.product ? `<div><span class="label">Product:</span> ${{port.product}}</div>` : ""}}
                        ${{port.version ? `<div><span class="label">Version:</span> ${{port.version}}</div>` : ""}}
                        ${{port.extra ? `<div><span class="label">Extra:</span> ${{port.extra}}</div>` : ""}}
                    </div>
                `).join("");
            }} else {{
                portsHtml = `<p class="muted">No open ports found in this XML.</p>`;
            }}

            let osHtml = "";
            if (host.os.length > 0) {{
                osHtml = host.os.map(os => `
                    <div class="os-card">
                        <div><span class="label">OS Guess:</span> ${{os.name}}</div>
                        <div><span class="label">Accuracy:</span> ${{os.accuracy}}%</div>
                    </div>
                `).join("");
            }} else {{
                osHtml = `<p class="muted">No OS guesses found.</p>`;
            }}

            details.innerHTML = `
                <h1>${{host.ip}}</h1>

                <div class="section">
                    <div><span class="label">Hostname:</span> ${{host.hostname}}</div>
                    <div><span class="label">MAC:</span> ${{host.mac}}</div>
                    <div><span class="label">Vendor:</span> ${{host.vendor}}</div>
                </div>

                <div class="section">
                    <h2>Open Ports</h2>
                    ${{portsHtml}}
                </div>

                <div class="section">
                    <h2>OS Detection</h2>
                    ${{osHtml}}
                </div>
            `;
        }});
    </script>
</body>
</html>
"""

    Path(output_file).write_text(html, encoding="utf-8")


def main():
    if len(sys.argv) != 2:
        print(f"Usage: python3 {sys.argv[0]} scan.xml")
        sys.exit(1)

    xml_file = sys.argv[1]

    try:
        hosts = parse_nmap_xml(xml_file)
    except ET.ParseError as e:
        print(f"Failed to parse XML: {e}")
        print("Your Nmap XML is probably incomplete. Let the scan finish and try again.")
        sys.exit(1)

    if not hosts:
        print("No completed host scan entries were found in the XML.")
        sys.exit(1)

    build_html(hosts)
    print("Created: nmap_interactive_map.html")


if __name__ == "__main__":
    main()