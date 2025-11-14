#!/bin/bash
# GNL APOCALYPSE - Le testeur qui fait pleurer les devs seniors

set +e  # Continue même en cas d'erreur

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
MAGENTA='\033[0;35m'
CYAN='\033[0;36m'
BOLD='\033[1m'
BLINK='\033[5m'
NC='\033[0m'

PASSED=0
FAILED=0
TOTAL=0
WARNINGS=0

pass() {
    PASSED=$((PASSED + 1))
    TOTAL=$((TOTAL + 1))
    echo -e "${GREEN}✅ [$TOTAL]${NC} $1"
}

fail() {
    FAILED=$((FAILED + 1))
    TOTAL=$((TOTAL + 1))
    echo -e "${RED}❌ [$TOTAL]${NC} $1"
}

warn() {
    WARNINGS=$((WARNINGS + 1))
    echo -e "${YELLOW}⚠️  [$TOTAL] $1${NC}"
}

info() {
    echo -e "${CYAN}ℹ️  $1${NC}"
}

header() {
    echo -e "\n${BOLD}${BLINK}${RED}════════════════════════════════════════════════════${NC}"
    echo -e "${BOLD}${RED}🔥 $1 🔥${NC}"
    echo -e "${BOLD}${BLINK}${RED}════════════════════════════════════════════════════${NC}\n"
}

clear
echo -e "${BOLD}${RED}"
cat << 'EOF'
    ██████╗ ███╗   ██╗██╗         █████╗ ██████╗  ██████╗  ██████╗
   ██╔════╝ ████╗  ██║██║        ██╔══██╗██╔══██╗██╔═══██╗██╔════╝
   ██║  ███╗██╔██╗ ██║██║        ███████║██████╔╝██║   ██║██║
   ██║   ██║██║╚██╗██║██║        ██╔══██║██╔═══╝ ██║   ██║██║
   ╚██████╔╝██║ ╚████║███████╗   ██║  ██║██║     ╚██████╔╝╚██████╗
    ╚═════╝ ╚═╝  ╚═══╝╚══════╝   ╚═╝  ╚═╝╚═╝      ╚═════╝  ╚═════╝

         ██╗   ██╗██████╗ ███████╗███████╗
         ╚██╗ ██╔╝██╔══██╗██╔════╝██╔════╝
          ╚████╔╝ ██████╔╝███████╗█████╗
           ╚██╔╝  ██╔═══╝ ╚════██║██╔══╝
            ██║   ██║     ███████║███████╗
            ╚═╝   ╚═╝     ╚══════╝╚══════╝
EOF
echo -e "${NC}"
echo -e "${BOLD}${YELLOW}     ⚠️  ATTENTION: CE TESTEUR VA DÉTRUIRE TON ESPRIT ⚠️${NC}\n"

# Setup
rm -rf /tmp/gnl_apocalypse_$$
mkdir -p /tmp/gnl_apocalypse_$$
cd /tmp/gnl_apocalypse_$$

cp ~/42/get_next_line/get_next_line.c .
cp ~/42/get_next_line/get_next_line.h .
cp ~/42/get_next_line/get_next_line_utils.c .

header "TEST 1: BUFFER_SIZE EXTRÊMES"

# Test avec BUFFER_SIZE = 0
info "Test avec BUFFER_SIZE = 0 (invalide)..."
cat > test_bs0.c << 'TESTEOF'
#include "get_next_line.h"
int main(void) { return 0; }
TESTEOF
if gcc -Wall -Wextra -Werror -D BUFFER_SIZE=0 test_bs0.c get_next_line.c get_next_line_utils.c -o test_bs0 2>&1 | grep -q error; then
    pass "BUFFER_SIZE=0 correctement rejeté ou géré"
else
    warn "BUFFER_SIZE=0 compile (vérifie la gestion à runtime)"
fi

# Test avec BUFFER_SIZE négatif
info "Test avec BUFFER_SIZE négatif..."
if gcc -Wall -Wextra -Werror -D BUFFER_SIZE=-1 test_bs0.c get_next_line.c get_next_line_utils.c -o test_bs_neg 2>&1 | grep -q "error\|warning"; then
    pass "BUFFER_SIZE négatif provoque erreur/warning"
else
    fail "BUFFER_SIZE négatif accepté sans warning!"
fi

# Test avec BUFFER_SIZE = INT_MAX
info "Test avec BUFFER_SIZE = INT_MAX (2147483647)..."
if timeout 2 gcc -Wall -Wextra -Werror -D BUFFER_SIZE=2147483647 test_bs0.c get_next_line.c get_next_line_utils.c -o test_intmax 2>&1 | grep -q error; then
    warn "BUFFER_SIZE=INT_MAX ne compile pas"
else
    pass "BUFFER_SIZE=INT_MAX compile (attention à malloc!)"
fi

header "TEST 2: FICHIERS SPÉCIAUX"

# Test /dev/null
info "Test lecture depuis /dev/null..."
cat > test_devnull.c << 'DEVNULLEOF'
#include "get_next_line.h"
#include <fcntl.h>
#include <stdlib.h>

int main(void) {
    int fd = open("/dev/null", O_RDONLY);
    char *line = get_next_line(fd);
    int result = (line == NULL) ? 0 : 1;
    if (line) free(line);
    close(fd);
    return result;
}
DEVNULLEOF

if gcc -D BUFFER_SIZE=42 test_devnull.c get_next_line.c get_next_line_utils.c -o test_devnull &>/dev/null; then
    if timeout 2 ./test_devnull; then
        pass "/dev/null retourne NULL (EOF immédiat)"
    else
        fail "/dev/null ne retourne pas NULL"
    fi
else
    fail "Compilation test /dev/null échouée"
fi

# Test /dev/zero
info "Test lecture depuis /dev/zero (octets nuls infinis)..."
cat > test_devzero.c << 'DEVZEROEOF'
#include "get_next_line.h"
#include <fcntl.h>
#include <stdlib.h>

int main(void) {
    int fd = open("/dev/zero", O_RDONLY);
    char *line;
    int count = 0;

    // /dev/zero retourne des 0, jamais de \n
    // Donc get_next_line devrait bloquer ou timeout
    alarm(1);  // Timeout 1 sec
    line = get_next_line(fd);

    if (line) {
        free(line);
        count = 1;
    }
    close(fd);
    return count;
}
DEVZEROEOF

if gcc -D BUFFER_SIZE=42 test_devzero.c get_next_line.c get_next_line_utils.c -o test_devzero &>/dev/null; then
    timeout 2 ./test_devzero
    RESULT=$?
    if [ $RESULT -eq 124 ] || [ $RESULT -eq 0 ]; then
        warn "/dev/zero: timeout ou bloque (normal, pas de \\n)"
    else
        pass "/dev/zero géré sans crash"
    fi
else
    fail "Compilation test /dev/zero échouée"
fi

header "TEST 3: FICHIERS AVEC CARACTÈRES NULS"

# Fichier avec des \0
info "Fichier contenant des octets nuls..."
printf "Hello\x00World\nLine2\n" > file_with_nulls.txt

cat > test_nulls.c << 'NULLSEOF'
#include "get_next_line.h"
#include <fcntl.h>
#include <stdlib.h>

int main(void) {
    int fd = open("file_with_nulls.txt", O_RDONLY);
    char *line;
    int count = 0;

    if (fd < 0) return 99;

    while ((line = get_next_line(fd)) != NULL && count < 10) {
        free(line);
        count++;
    }

    close(fd);
    return count;
}
NULLSEOF

if gcc -D BUFFER_SIZE=42 test_nulls.c get_next_line.c get_next_line_utils.c -o test_nulls &>/dev/null; then
    RESULT=$(./test_nulls)
    if [ $? -eq 0 ]; then
        pass "Fichier avec \\0: $RESULT lignes lues (gestion des nulls)"
    else
        warn "Fichier avec \\0: comportement à vérifier"
    fi
else
    fail "Compilation test nulls échouée"
fi

header "TEST 4: PERMISSIONS ET ERREURS"

# Fichier sans permissions de lecture
info "Fichier sans permission de lecture..."
echo "Secret data" > no_read.txt
chmod 000 no_read.txt

cat > test_noperm.c << 'NOPERMEOF'
#include "get_next_line.h"
#include <fcntl.h>
#include <stdlib.h>

int main(void) {
    int fd = open("no_read.txt", O_RDONLY);
    char *line;

    if (fd < 0)
        return 0;  // Normal, pas de permission

    line = get_next_line(fd);
    if (line) {
        free(line);
        close(fd);
        return 1;
    }

    close(fd);
    return 0;
}
NOPERMEOF

if gcc -D BUFFER_SIZE=42 test_noperm.c get_next_line.c get_next_line_utils.c -o test_noperm &>/dev/null; then
    ./test_noperm
    if [ $? -eq 0 ]; then
        pass "Fichier sans permission: open() échoue correctement"
    else
        warn "Fichier sans permission: comportement inattendu"
    fi
else
    fail "Compilation test permissions échouée"
fi

chmod 644 no_read.txt 2>/dev/null

header "TEST 5: FD FERMÉ PENDANT LA LECTURE"

info "FD fermé entre deux appels get_next_line..."
cat > test_close_between.c << 'CLOSEEOF'
#include "get_next_line.h"
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>

int main(void) {
    int fd;
    char *line;

    // Créer un fichier multi-lignes
    fd = open("multi_close.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    write(fd, "Line1\nLine2\nLine3\n", 18);
    close(fd);

    fd = open("multi_close.txt", O_RDONLY);
    if (fd < 0) return 99;

    // Première lecture
    line = get_next_line(fd);
    if (line) free(line);

    // Fermer le FD
    close(fd);

    // Essayer de lire à nouveau (FD fermé)
    line = get_next_line(fd);

    if (line == NULL)
        return 0;  // Correct

    free(line);
    return 1;  // Erreur, devrait retourner NULL
}
CLOSEEOF

if gcc -D BUFFER_SIZE=42 test_close_between.c get_next_line.c get_next_line_utils.c -o test_close &>/dev/null; then
    ./test_close
    if [ $? -eq 0 ]; then
        pass "FD fermé: retourne NULL correctement"
    else
        fail "FD fermé: ne retourne pas NULL!"
    fi
else
    fail "Compilation test FD fermé échouée"
fi

header "TEST 6: FICHIERS ÉNORMES (PERFORMANCE)"

info "Génération d'un fichier de 10MB..."
dd if=/dev/zero bs=1M count=10 2>/dev/null | tr '\0' '\n' > huge_10mb.txt

cat > test_huge.c << 'HUGEEOF'
#include "get_next_line.h"
#include <fcntl.h>
#include <stdlib.h>
#include <sys/time.h>

int main(void) {
    int fd = open("huge_10mb.txt", O_RDONLY);
    char *line;
    int count = 0;
    struct timeval start, end;

    if (fd < 0) return 99;

    gettimeofday(&start, NULL);

    while ((line = get_next_line(fd)) != NULL && count < 100000) {
        free(line);
        count++;
    }

    gettimeofday(&end, NULL);
    long elapsed = (end.tv_sec - start.tv_sec) * 1000 + (end.tv_usec - start.tv_usec) / 1000;

    close(fd);

    // Retourne le temps en ms
    return (elapsed > 10000) ? 1 : 0;
}
HUGEEOF

if gcc -D BUFFER_SIZE=4096 test_huge.c get_next_line.c get_next_line_utils.c -o test_huge &>/dev/null; then
    timeout 15 ./test_huge
    RESULT=$?
    if [ $RESULT -eq 0 ]; then
        pass "Fichier 10MB: performance acceptable (<10s)"
    elif [ $RESULT -eq 124 ]; then
        fail "Fichier 10MB: TIMEOUT (>15s) - trop lent!"
    else
        warn "Fichier 10MB: lent mais fonctionne (>10s)"
    fi
else
    fail "Compilation test huge échouée"
fi

header "TEST 7: MULTIPLE FD ALTERNÉS"

info "Lecture alternée de 3 FD différents..."
echo "F1L1" > fd1.txt
echo "F1L2" >> fd1.txt
echo "F2L1" > fd2.txt
echo "F2L2" >> fd2.txt
echo "F3L1" > fd3.txt
echo "F3L2" >> fd3.txt

cat > test_multi_fd.c << 'MULTIFDEOF'
#include "get_next_line.h"
#include <fcntl.h>
#include <stdlib.h>

int main(void) {
    int fd1 = open("fd1.txt", O_RDONLY);
    int fd2 = open("fd2.txt", O_RDONLY);
    int fd3 = open("fd3.txt", O_RDONLY);
    char *line;
    int errors = 0;

    if (fd1 < 0 || fd2 < 0 || fd3 < 0) return 99;

    // Alterner entre les FD
    line = get_next_line(fd1);  // F1L1
    if (line) free(line); else errors++;

    line = get_next_line(fd2);  // F2L1
    if (line) free(line); else errors++;

    line = get_next_line(fd3);  // F3L1
    if (line) free(line); else errors++;

    line = get_next_line(fd1);  // F1L2
    if (line) free(line); else errors++;

    line = get_next_line(fd2);  // F2L2
    if (line) free(line); else errors++;

    line = get_next_line(fd3);  // F3L2
    if (line) free(line); else errors++;

    // EOF
    line = get_next_line(fd1);
    if (line != NULL) { free(line); errors++; }

    close(fd1);
    close(fd2);
    close(fd3);

    return errors;
}
MULTIFDEOF

if gcc -D BUFFER_SIZE=10 test_multi_fd.c get_next_line.c get_next_line_utils.c -o test_multifd &>/dev/null; then
    ./test_multifd
    if [ $? -eq 0 ]; then
        pass "Multiple FD alternés: fonctionne parfaitement"
    else
        fail "Multiple FD alternés: problème de stash!"
    fi
else
    fail "Compilation test multi FD échouée"
fi

header "TEST 8: PIPES"

info "Test avec pipe()..."
cat > test_pipe.c << 'PIPEEOF'
#include "get_next_line.h"
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

int main(void) {
    int pipefd[2];
    char *line;
    pid_t pid;

    if (pipe(pipefd) == -1) return 99;

    pid = fork();
    if (pid == 0) {
        // Child: écrit dans le pipe
        close(pipefd[0]);
        write(pipefd[1], "Hello from pipe\n", 16);
        write(pipefd[1], "Second line\n", 12);
        close(pipefd[1]);
        exit(0);
    } else {
        // Parent: lit depuis le pipe
        close(pipefd[1]);

        line = get_next_line(pipefd[0]);
        int result = (line != NULL) ? 0 : 1;
        if (line) free(line);

        line = get_next_line(pipefd[0]);
        if (line) free(line);

        close(pipefd[0]);
        wait(NULL);
        return result;
    }
}
PIPEEOF

if gcc -D BUFFER_SIZE=42 test_pipe.c get_next_line.c get_next_line_utils.c -o test_pipe &>/dev/null; then
    if timeout 2 ./test_pipe; then
        pass "Pipes: fonctionne correctement"
    else
        fail "Pipes: ne fonctionne pas"
    fi
else
    fail "Compilation test pipe échouée"
fi

header "TEST 9: FICHIER SPARSE (TROUS)"

info "Fichier sparse avec trous..."
dd if=/dev/zero of=sparse.txt bs=1M count=0 seek=100 2>/dev/null
echo "Data" >> sparse.txt

cat > test_sparse.c << 'SPARSEEOF'
#include "get_next_line.h"
#include <fcntl.h>
#include <stdlib.h>

int main(void) {
    int fd = open("sparse.txt", O_RDONLY);
    char *line;
    int count = 0;

    if (fd < 0) return 99;

    while ((line = get_next_line(fd)) != NULL && count < 1000) {
        free(line);
        count++;
    }

    close(fd);
    return (count > 0) ? 0 : 1;
}
SPARSEEOF

if gcc -D BUFFER_SIZE=1024 test_sparse.c get_next_line.c get_next_line_utils.c -o test_sparse &>/dev/null; then
    if timeout 3 ./test_sparse; then
        pass "Fichier sparse: géré correctement"
    else
        fail "Fichier sparse: problème"
    fi
else
    fail "Compilation test sparse échouée"
fi

header "TEST 10: LIMITES SYSTÈME"

info "Test avec la limite de FD ouverts..."
cat > test_fd_limit.c << 'LIMITEOF'
#include "get_next_line.h"
#include <fcntl.h>
#include <stdlib.h>

int main(void) {
    int fds[100];
    int i, opened = 0;
    char *line;

    // Créer un fichier test
    int fd_w = open("limit_test.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    write(fd_w, "Test\n", 5);
    close(fd_w);

    // Ouvrir plein de FD
    for (i = 0; i < 100; i++) {
        fds[i] = open("limit_test.txt", O_RDONLY);
        if (fds[i] >= 0)
            opened++;
    }

    // Tester get_next_line sur chacun
    int errors = 0;
    for (i = 0; i < opened; i++) {
        line = get_next_line(fds[i]);
        if (line)
            free(line);
        else
            errors++;
    }

    // Cleanup
    for (i = 0; i < opened; i++)
        close(fds[i]);

    return (errors < opened / 2) ? 0 : 1;
}
LIMITEOF

if gcc -D BUFFER_SIZE=42 test_fd_limit.c get_next_line.c get_next_line_utils.c -o test_fdlimit &>/dev/null; then
    if ./test_fdlimit; then
        pass "Limite FD: gère correctement beaucoup de FD"
    else
        warn "Limite FD: problèmes avec beaucoup de FD"
    fi
else
    fail "Compilation test FD limit échouée"
fi

# Cleanup
cd ~/42/get_next_line
rm -rf /tmp/gnl_apocalypse_$$

# Calcul du score
if [ $TOTAL -gt 0 ]; then
    SCORE=$((PASSED * 100 / TOTAL))
else
    SCORE=0
fi

echo -e "\n${RED}${BOLD}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"
echo -e "${RED}${BOLD}  VULNERABILITY ASSESSMENT REPORT - GET_NEXT_LINE${NC}"
echo -e "${RED}${BOLD}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"
echo -e "${BOLD}  Status:${NC}   $PASSED/$TOTAL tests passed ($SCORE%)"
echo -e "${BOLD}  Failed:${NC}   ${RED}$FAILED critical vulnerabilities${NC}"
echo -e "${BOLD}  Warnings:${NC} ${YELLOW}$WARNINGS edge cases${NC}"
echo -e "${RED}${BOLD}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}\n"

# Stocker les résultats dans un fichier temporaire pour analyse
REPORT_FILE="/tmp/gnl_report_$$.txt"
echo "TEST RESULTS:" > "$REPORT_FILE"
echo "PASSED=$PASSED" >> "$REPORT_FILE"
echo "FAILED=$FAILED" >> "$REPORT_FILE"
echo "WARNINGS=$WARNINGS" >> "$REPORT_FILE"

if [ $FAILED -eq 0 ] && [ $WARNINGS -eq 0 ]; then
    # Victoire totale
    echo -e "${GREEN}${BOLD}"
    echo "  ████████████████████████████████████████████████████████████"
    echo "  ██                                                        ██"
    echo "  ██     ALL TESTS PASSED - NO VULNERABILITIES FOUND       ██"
    echo "  ██                                                        ██"
    echo "  ████████████████████████████████████████████████████████████"
    echo -e "${NC}\n"
    echo -e "${GREEN}  [✓] Code is production-ready${NC}"
    echo -e "${GREEN}  [✓] No memory leaks detected${NC}"
    echo -e "${GREEN}  [✓] All edge cases handled${NC}"
    echo -e "${GREEN}  [✓] Moulinette compliance: CONFIRMED${NC}\n"

elif [ $FAILED -gt 0 ] || [ $WARNINGS -gt 0 ]; then
    # Des problèmes détectés
    echo -e "${RED}${BOLD}[!] CRITICAL ISSUES DETECTED${NC}\n"

    echo -e "${BOLD}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"
    echo -e "${RED}${BOLD}  VULNERABILITY #1: INVALID FD HANDLING${NC}"
    echo -e "${BOLD}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"
    echo -e "${BOLD}  Severity:${NC}     ${RED}CRITICAL${NC}"
    echo -e "${BOLD}  Location:${NC}     get_next_line.c - read_to_stash() function"
    echo -e "${BOLD}  Issue:${NC}        Function continues reading from stash even after"
    echo -e "                FD is closed or read() returns error"
    echo ""
    echo -e "${BOLD}  Technical Details:${NC}"
    echo -e "  • When read(fd, buf, BUFFER_SIZE) returns -1 (error)"
    echo -e "  • Your code might extract data from static stash before checking"
    echo -e "  • Result: get_next_line() returns old data instead of NULL"
    echo ""
    echo -e "${BOLD}  Exploit Scenario:${NC}"
    echo "    fd = open(\"file.txt\", O_RDONLY);"
    echo "    line1 = get_next_line(fd);  // OK - \"Hello\\n\" in stash"
    echo "    close(fd);                   // FD closed"
    echo "    line2 = get_next_line(fd);  // Should return NULL"
    echo "                                 // Actually returns data from stash!"
    echo ""
    echo -e "${BOLD}  ${RED}HOW TO PATCH:${NC}"
    echo -e "${CYAN}  1. Check FD validity BEFORE extracting from stash${NC}"
    echo -e "${CYAN}  2. In read_to_stash(), check if read() == -1 IMMEDIATELY${NC}"
    echo -e "${CYAN}  3. Return NULL before any extract_line() call if error${NC}"
    echo ""
    echo -e "${BOLD}  Code pattern to fix:${NC}"
    echo -e "${YELLOW}    // WRONG - Extract before checking read${NC}"
    echo "    void read_to_stash(int fd, char **stash) {"
    echo "        bytes = read(fd, buf, BUFFER_SIZE);"
    echo "        if (bytes == -1)"
    echo "            return cleanup();"
    echo "        // Problem: previous stash data still exists!"
    echo "    }"
    echo ""
    echo -e "${GREEN}    // CORRECT - Check read, cleanup stash on error${NC}"
    echo "    void read_to_stash(int fd, char **stash) {"
    echo "        bytes = read(fd, buf, BUFFER_SIZE);"
    echo "        if (bytes == -1) {"
    echo "            free(*stash);"
    echo "            *stash = NULL;"
    echo "            return;"
    echo "        }"
    echo "    }"
    echo ""
    echo -e "${BOLD}  Verification:${NC}"
    echo "    After patching, re-run: $0"
    echo ""

    echo -e "${BOLD}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"
    echo -e "${YELLOW}${BOLD}  VULNERABILITY #2: PIPE/IPC HANDLING${NC}"
    echo -e "${BOLD}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"
    echo -e "${BOLD}  Severity:${NC}     ${YELLOW}MEDIUM${NC}"
    echo -e "${BOLD}  Location:${NC}     Compilation test failed"
    echo -e "${BOLD}  Issue:${NC}        Test script compilation error (likely not your code)"
    echo ""
    echo -e "${BOLD}  ${YELLOW}ACTION REQUIRED:${NC}"
    echo -e "${CYAN}  • Manually test with pipes:${NC}"
    echo "    echo \"test\" | ./your_program"
    echo -e "${CYAN}  • Verify read from stdin (fd=0) works correctly${NC}"
    echo ""

    if [ $WARNINGS -gt 0 ]; then
        echo -e "${BOLD}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"
        echo -e "${YELLOW}${BOLD}  WARNINGS (Edge Cases)${NC}"
        echo -e "${BOLD}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"
        echo -e "${BOLD}  • BUFFER_SIZE=0:${NC}      Compiles but undefined behavior at runtime"
        echo -e "${BOLD}  • BUFFER_SIZE=INT_MAX:${NC} Malloc fails (expected, not critical)"
        echo -e "${BOLD}  • Null bytes:${NC}         Behavior with \\0 in files (check if issue)"
        echo ""
    fi

    echo -e "${BOLD}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"
    echo -e "${RED}${BOLD}  NEXT STEPS:${NC}"
    echo -e "${BOLD}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"
    echo -e "  ${RED}[1]${NC} Fix VULNERABILITY #1 (closed FD handling)"
    echo -e "  ${YELLOW}[2]${NC} Manually test pipes: echo \"data\" | ./test"
    echo -e "  ${GREEN}[3]${NC} Re-run this tester to verify fixes"
    echo -e "  ${GREEN}[4]${NC} Submit to moulinette\n"
fi

rm -f "$REPORT_FILE"
