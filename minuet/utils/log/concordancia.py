import os
import collections

def process_log_file(filepath):
    """
    Lê um arquivo de log, agrupa por timestamp e encontra o NodeID com o maior FinalScore para cada timestamp.
    Retorna um dicionário: {timestamp: best_node_id}
    """
    print(f"Processando arquivo: {filepath}...")
    
    decisions_at_time = collections.defaultdict(list)
    
    try:
        with open(filepath, 'r') as f:
            # Pula o cabeçalho
            next(f)
            
            for line in f:
                try:
                    parts = line.strip().split(',')
                    if len(parts) < 16: # Garante que a linha tem todos os campos
                        continue
                        
                    timestamp = int(parts[0])
                    node_id = int(parts[1])
                    final_score = float(parts[-1]) # O último valor é sempre o score
                    
                    decisions_at_time[timestamp].append({'node_id': node_id, 'score': final_score})
                except (ValueError, IndexError):
                    # Ignora linhas malformadas
                    continue

    except FileNotFoundError:
        print(f"  - ERRO: Arquivo não encontrado: {filepath}")
        return {}

    # Agora, para cada timestamp, encontre a melhor decisão
    best_choices = {}
    for timestamp, candidates in decisions_at_time.items():
        if candidates:
            # Encontra o dicionário com o maior score usando uma função lambda
            best_candidate = max(candidates, key=lambda x: x['score'])
            best_choices[timestamp] = best_candidate['node_id']
            
    print(f"  - Análise concluída. {len(best_choices)} pontos de decisão encontrados.")
    return best_choices

# --- BLOCO PRINCIPAL DE EXECUÇÃO ---
if __name__ == "__main__":
    
    # =======================  CONFIGURAÇÃO  =========================
    LOG_FILES = {
        "AHP": "score_history_AHP.csv",
        "TOPSIS": "score_history_TOPSIS.csv",
        "PROMETHEE": "score_history_PROMETHEE.csv"
    }
    # =================================================================

    all_best_choices = {}
    all_timestamps = set()

    # Passo 1: Processar todos os arquivos e coletar as melhores escolhas
    method_names = list(LOG_FILES.keys())
    for method in method_names:
        filename = LOG_FILES[method]
        if os.path.exists(filename):
            best_choices = process_log_file(filename)
            all_best_choices[method] = best_choices
            all_timestamps.update(best_choices.keys())
        else:
            print(f"Aviso: O arquivo '{filename}' não foi encontrado. O método {method} será ignorado.")

    if not all_best_choices:
        print("\nNenhum arquivo de log válido foi encontrado. Encerrando.")
    else:
        # Passo 2: Comparar as decisões para cada timestamp
        agreement_counts = collections.defaultdict(int)
        pairwise_comparison_points = 0
        three_way_comparison_points = 0
        
        # Define os pares para comparação
        pairs_to_compare = [
            (method_names[0], method_names[1]),
            (method_names[0], method_names[2]),
            (method_names[1], method_names[2])
        ]

        for timestamp in sorted(list(all_timestamps)):
            # Só compara se tivermos uma decisão de pelo menos dois métodos para este timestamp
            methods_present = [m for m in all_best_choices if timestamp in all_best_choices[m]]
            
            # --- LÓGICA PARA CONCORDÂNCIA PAR A PAR ---
            if len(methods_present) >= 2:
                pairwise_comparison_points += 1 # Contamos como um ponto de decisão válido para pares

                for method1, method2 in pairs_to_compare:
                    if method1 in methods_present and method2 in methods_present:
                        choice1 = all_best_choices[method1][timestamp]
                        choice2 = all_best_choices[method2][timestamp]
                        
                        if choice1 == choice2:
                            agreement_counts[f"{method1}_vs_{method2}"] += 1
            
            # --- LÓGICA PARA CONCORDÂNCIA TRIPLA ---
            if len(methods_present) == 3:
                three_way_comparison_points += 1 # Ponto válido para comparação tripla
                
                choice1 = all_best_choices[method_names[0]][timestamp]
                choice2 = all_best_choices[method_names[1]][timestamp]
                choice3 = all_best_choices[method_names[2]][timestamp]

                if choice1 == choice2 == choice3:
                    agreement_counts["Todos_os_3"] += 1


        # Passo 3: Calcular e imprimir os resultados
        print("\n--- Relatório de Concordância na Seleção de Relay ---")
        
        # --- TABELA DE CONCORDÂNCIA PAR A PAR ---
        if pairwise_comparison_points == 0:
            print("Nenhum ponto de decisão em comum foi encontrado para comparar.")
        else:
            print(f"\nTotal de pontos de decisão para comparações par a par: {pairwise_comparison_points}\n")
            
            print("--- Concordância Par a Par ---")
            print("---------------------------------------------------")
            print("| Comparação          | Concordância  | Porcentagem |")
            print("---------------------------------------------------")
            
            for method1, method2 in pairs_to_compare:
                key = f"{method1}_vs_{method2}"
                count = agreement_counts[key]
                percentage = (count / pairwise_comparison_points) * 100 if pairwise_comparison_points > 0 else 0
                
                print(f"| {method1:<9} vs. {method2:<9} | {count:<13} | {percentage:10.2f}% |")
            
            print("---------------------------------------------------")

        # --- RESULTADO DA CONCORDÂNCIA TRIPLA ---
        if three_way_comparison_points == 0:
            print("\nNenhum ponto de decisão encontrado onde todos os 3 métodos estivessem presentes.")
        else:
            print(f"\nTotal de pontos de decisão onde os 3 métodos estavam presentes: {three_way_comparison_points}\n")
            print("--- Concordância Tripla ---")
            print("-----------------------------------------------------------------")
            print("| Comparação                  | Concordância  | Porcentagem     |")
            print("-----------------------------------------------------------------")
            
            count = agreement_counts["Todos_os_3"]
            percentage = (count / three_way_comparison_points) * 100 if three_way_comparison_points > 0 else 0
            
            comparison_label = f"{method_names[0]} vs. {method_names[1]} vs. {method_names[2]}"
            print(f"| {comparison_label:<27} | {count:<13} | {percentage:10.2f}%       |")
            print("-----------------------------------------------------------------")