function Duty = MPPT(Vpv,Ipv)

    persistent D; 
    persistent V;
    persistent D_PB;
    persistent P_PB;
    persistent D_GB;
    persistent P_GB;
    persistent D_MAX; 
    persistent D_MIN; 
    persistent V_MAX; 
    persistent V_MIN; 
    persistent D_PREV;
    persistent W; 
    persistent ITER; 
    persistent P;
    persistent FLAG_ZERO; 
    persistent FLAG_CONV;
    persistent FLAG_RESET;
    persistent DELAY;
    persistent AVG;
    persistent Ppv;
    persistent Pprev;
    
    
    if isempty(FLAG_RESET) && isempty(Ppv)
        FLAG_RESET = 0;
        Ppv = 0;
    end
    
    l_limit = 0;
    u_limit = 1;
    C1 = 2.05;
    C2 = 2.05;
    C3 = 10;
    C4 = 0.0025;
    beta = 0.1;

    sigma = 0.1;
    wmax = 1;
    wmin = 0.4;

    T_MAX = 10; % Maximum Iterations
    M = 10; % Number of Particles
    SET = 2000;
    PET = SET * 0.5; %0.25

    if(isempty(D) || FLAG_RESET == 1)
        rng('shuffle');
        D = divideDuty(M,u_limit,l_limit);
        D_PREV = D;
        V = zeros(M,1);
        D_PB = D;
        D_GB = 0; % Maximizing Objective Function
        P_PB = zeros(M,1);
        P_GB = 0;
        
        D_MIN = zeros(M,1);
        D_MAX = zeros(M,1);
        D_MAX(:,:) = 1;
        V_MIN = zeros(M,1);
        V_MIN = -1 * beta * (D_MAX - D_MIN);
        V_MAX = zeros(M,1);
        V_MAX = beta * (D_MAX - D_MIN);
        
        W = zeros(M,1);
        W(:,:) = wmax;
        ITER = 0; % Initial Iteration
        P = 1; % Initial Particle
        
        FLAG_ZERO = 0;
        FLAG_CONV = 0;
        FLAG_RESET = 0;
        DELAY = 1;
        AVG = 0;
        Duty = D(P); % Initial Duty Cycle P = 1
        P = P + 1;
        return;
    end
    
    % Delay for settling
    if DELAY <= SET && FLAG_CONV == 0
        if DELAY >= PET % Averages the Power obtained
            AVG = AVG + (Vpv * Ipv);
        end
        Duty = D(P-1);
        DELAY = DELAY + 1;
        return;
    end 

    if FLAG_CONV == 1 % Checking for Reset Criteria
        Pprev = Ppv;
        Ppv = Vpv * Ipv;
        abs((Ppv - Pprev) / Pprev);
        if abs((Ppv - Pprev) / Pprev) > 0.1
            FLAG_RESET = 1;
            disp("Reset Flag Raised");
        end
        Duty = D_GB;
        return;
    end
    
    Pprev = Ppv;
    Ppv = AVG / (SET - PET); % Objective Function
    AVG = 0;
    
    if ITER == 0 % Zero Pass
        P_PB(P-1) = Ppv; 
        % Personal Best Power, previous particle duty cycle evaluated
        if Ppv > P_GB
            D_GB = D(P-1);
            P_GB = Ppv;
        end
        if P == M + 1
            disp("Zero Flag Raised");
            FLAG_ZERO = 1;
            P = 1;
        else
            Duty = D(P); 
            P = P + 1;
            DELAY = 1;
            return;
        end
    end

    if FLAG_CONV == 0 && ITER <= T_MAX 
        if FLAG_ZERO == 0
            if Ppv > P_PB(P-1)
                D_PB(P-1) = D(P-1);
                P_PB(P-1) = Ppv;
            end
            if Ppv > P_GB
                D_GB = D(P-1);
                P_GB = Ppv;
            end
        end

        if P == M + 1 || FLAG_ZERO == 1 
            % Updating Positions and Velocities
            
            % Limits Updation
            D_MAX = D_MAX - sigma * (D_MAX - D_GB);
            D_MIN = D_MIN + sigma * (D_GB - D_MIN);
            V_MIN = -1 * beta * (D_MAX - D_MIN);
            V_MAX = beta * (D_MAX - D_MIN);
            
            % Velocity
            R1 = rand(M,1);
            R2 = rand(M,1);
            R3 = rand(M,1);
            R4 = rand(M,1);
            V = (W .* V) + (C1 * (R1 .* (D_PB - D))) + (C2 * (R2 .* (D_GB - D))); 
            W_temp = wmax - ((wmax - wmin) / T_MAX) * ITER; 
            W(:,:) = W_temp;
            V = V + (C3 * (R3 .* (D_GB + D_PB - 2 * D))) + (C4 * (R4 .* (D_GB - D_PB)));
            
            % Velocity Limits
            idx_min = find(V < V_MIN);
            V(idx_min) = V_MIN(idx_min);
            idx_max = find(V > V_MAX);
            V(idx_max) = V_MAX(idx_max);
            
            % Position Update
            D_PREV = D;
            D = D + V;
            
            % Position Limits
            idx_min = find(D < D_MIN);
            D(idx_min) = D_MIN(idx_min);
            idx_max = find(D > D_MAX);
            D(idx_max) = D_MAX(idx_max);
            
            if (abs((D_PREV - D) ./ D_PREV) < 0.01) 
                disp("Convergence Flag Raised");
                Duty = D_GB;
                FLAG_CONV = 1;
                return;
            elseif ITER >= T_MAX
                disp("Iteration Flag Raised");
                Duty = D_GB;
                FLAG_CONV = 1;
                return;
            end
            
            ITER = ITER + 1;
            P = 1;
            FLAG_ZERO = 0;
            
            % Restart Function Evaluation
            Duty = D(P);
            P = P + 1;
            DELAY = 1;
        else
            Duty = D(P);
            P = P + 1;
            DELAY = 1;
        end
    else
        Duty = D_GB;
        DELAY = 1;
    end
end

% This function randomly initializes the particles uniformly 
% across the 0 and 1 interval
function D = divideDuty(M,U,L)
    step = (U - L) / M;
    P = zeros(M,1);
    init_L = L;
    init_U = init_L + step;
    for i = 1:M
        P(i) = init_L + (init_U - init_L) * rand;
        init_L = init_U;
        init_U = init_L + step;
    end
    D = P;
end
