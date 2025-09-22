import matplotlib.pyplot as plt
import re
import os
import numpy as np

def parse_log_file_per_bs(filepath):
    """
    Lê um arquivo de log, conta as mensagens ÚNICAS para CADA estação base.
    Retorna um dicionário: {bs_id_1: count_1, bs_id_2: count_2, ...}
    """
    print(f"Analisando o arquivo: {filepath}...")
    
    messages_per_bs = {}
    
    log_pattern = re.compile(
        r'^(?P<time_ns>\d+)ns - BASE STATION - Node #(?P<bs_id>\d+): '
        r'Monitoring Message Received: .*?'
        r'MonitorId = (?P<monitor_id>\d+).*?'
        r'Seq = (?P<seq>\d+).*?'
        r'EventId = (?P<event_id>\d+)'
    )

    try:
        with open(filepath, 'r') as f:
            for line in f:
                match = log_pattern.search(line)
                if match:
                    data = match.groupdict()
                    bs_id = int(data['bs_id'])
                    
                    message_id = (
                        int(data['monitor_id']),
                        int(data['event_id']),
                        int(data['seq'])
                    )
                    
                    if bs_id not in messages_per_bs:
                        messages_per_bs[bs_id] = set()
                    
                    messages_per_bs[bs_id].add(message_id)

    except FileNotFoundError:
        print(f"ERRO: Arquivo não encontrado: {filepath}")
        return {}
        
    counts_per_bs = {bs_id: len(msgs) for bs_id, msgs in messages_per_bs.items()}
    
    print(f"Análise concluída. Contagens por BS: {counts_per_bs}")
    return counts_per_bs

def plot_grouped_bar_chart(results, bs0_id, bs1_id):
    """
    Cria um gráfico de barras agrupado e achatado, comparando os 3 métodos.
    """
    methods = list(results.keys())
    
    bs0_counts = [results[m].get(bs0_id, 0) for m in methods]
    bs1_counts = [results[m].get(bs1_id, 0) for m in methods]

    x = np.arange(len(methods))
    width = 0.35

    # --- ALTERAÇÃO PRINCIPAL AQUI ---
    # A altura foi reduzida de 8 para 6 para "achatar" o gráfico.
    # Você pode experimentar valores menores como 5 se quiser ainda mais achatado.
    fig, ax = plt.subplots(figsize=(12, 8))
    
    rects1 = ax.bar(x - width/2, bs0_counts, width, label=f'Base Station {bs0_id} (BS0)')
    rects2 = ax.bar(x + width/2, bs1_counts, width, label=f'Base Station {bs1_id} (BS1)')

    ax.bar_label(rects1, padding=3, weight='bold', fontsize=11)
    ax.bar_label(rects2, padding=3, weight='bold', fontsize=11)

    ax.set_title('Comparative Performance of MCDA Methods per Base Station', fontsize=16, pad=20)

    ax.set_ylabel('Total Messages Received', fontsize=14)
    ax.set_xticks(x)
    ax.set_xticklabels(methods, fontsize=12)
    ax.legend(fontsize=12)
    ax.spines['top'].set_visible(False)
    ax.spines['right'].set_visible(False)
    ax.yaxis.grid(True, linestyle='--', which='major', color='grey', alpha=0.7)
    
    # Ajusta o limite do eixo Y para dar um pouco mais de espaço aos rótulos no topo
    ax.set_ylim(0, max(max(bs0_counts), max(bs1_counts)) * 1.20)

    fig.tight_layout()

    # Salva o gráfico com um novo nome para não sobrescrever o antigo
    output_filename = "grafico_en.png"
    plt.savefig(output_filename, dpi=300)
    print(f"\nGráfico de barras formatado salvo como '{output_filename}'")
    
    plt.show()

# --- BLOCO PRINCIPAL DE EXECUÇÃO ---
if __name__ == "__main__":
    
    # =======================  CONFIGURAÇÃO  =========================
    # 1. Defina os IDs das suas estações base conforme seu artigo
    BASE_STATION_0_ID = 968 
    BASE_STATION_1_ID = 969

    # 2. Defina os nomes dos seus arquivos de log
    LOG_FILES = {
        "TOPSIS": "TOPSIS/logFileBaseStation.log",
        "AHP": "AHP/logFileBaseStation.log",
        "PROMETHEE": "PROMETHEE/logFileBaseStation.log"
    }
    # =================================================================

    all_results = {}
    
    for method, filename in LOG_FILES.items():
        if not os.path.exists(filename):
            print(f"\nAviso: O arquivo '{filename}' não foi encontrado. Pulando o método {method}.")
            continue
        
        counts_per_bs = parse_log_file_per_bs(filename)
        all_results[method] = counts_per_bs
        
    if all_results:
        # A ordem no gráfico será a mesma do dicionário LOG_FILES
        plot_grouped_bar_chart(all_results, BASE_STATION_0_ID, BASE_STATION_1_ID)
    else:
        print("\nNenhum dado para plotar. Verifique se os nomes dos arquivos estão corretos.")